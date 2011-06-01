#include "stdafx.h"
#include "RecvFrame.h"
#include "Resource.h"

CRecvFrame::CRecvFrame()
{
	m_hSemophere = ::CreateSemaphore(NULL,0,0x7FFFFFFF,NULL);
	::InitializeCriticalSection(&m_csFrame);
	::InitializeCriticalSection(&m_csBlock);
	m_hThread = NULL;
	m_ListenSocket= INVALID_SOCKET;
	m_fExit = 1;
	m_vSocketAddr.reserve(FD_SETSIZE);
	this->m_ThreadAddr = sThreadFunc;
	m_pMsgQueue = NULL;
}

CRecvFrame::~CRecvFrame()
{
	Stop();
	for (unsigned i = 0; i < m_vSocketAddr.size(); i++)
	{
		closesocket(m_vSocketAddr[i].s);
	}
	::CloseHandle(m_hSemophere);
	::DeleteCriticalSection(&m_csFrame);
	::DeleteCriticalSection(&m_csBlock);
}

void CRecvFrame::SetSockPort(unsigned short port)
{
	m_sPort = port;
}

unsigned short CRecvFrame::GetSockPort() const
{
	return m_sPort;
}

void CRecvFrame::SetMsgQueue(CSafeQueue<ConnMsg> * pQueue)
{
	m_pMsgQueue = pQueue;
}

HANDLE CRecvFrame::GetSemophere() const
{
	return m_hSemophere;
}

void CRecvFrame::AddBlockAddr(const char * szAddr)
{
	u_long uaddr = inet_addr(szAddr);
	::EnterCriticalSection(&m_csBlock);
	if (m_setBlock.insert(uaddr).second)
	{
		std::vector<Socket_Addr>::iterator iter = m_vSocketAddr.begin();
		while (iter != m_vSocketAddr.end())
		{
			if (((struct sockaddr_in *)&iter->addr)->sin_addr.s_addr == uaddr)
			{
				m_vSocketAddr.erase(iter);
				break;
			}
			iter++;
		}
	}
	::LeaveCriticalSection(&m_csBlock);
}

void CRecvFrame::DelBlockAddr(const char * szAddr)
{
	u_long uaddr = inet_addr(szAddr);
	::EnterCriticalSection(&m_csBlock);
	m_setBlock.erase(uaddr);
	::LeaveCriticalSection(&m_csBlock);
}


int CRecvFrame::Start()
{
	if (InitSocket())
	{
#ifdef _DEBUG
		::OutputDebugString(_T("RecvFrame: Init Socket Error"));
#endif
		return -1;
	}
	return IMyThread::Start();
}

std::string  CRecvFrame::GetFrame()
{
	std::string frame = "";
	::EnterCriticalSection(&m_csFrame);
	if (m_dqRecvedFrame.size())
	{
		frame = m_dqRecvedFrame.front();
		m_dqRecvedFrame.pop_front();
	}
	::LeaveCriticalSection(&m_csFrame);
	return frame;
}

unsigned int __stdcall CRecvFrame::sThreadFunc(void * p)
{
	CRecvFrame * pThis = dynamic_cast<CRecvFrame *>(reinterpret_cast<IMyThread *>(p));
	if (!pThis)
	{
#ifdef _DEBUG
		::OutputDebugString(_T("RecvFrame: Thread this Pointer Error"));
#endif
		return -1;
	}
	struct fd_set rfds;
	struct timeval t;
	t.tv_sec = THREADWAIT/1000;
	t.tv_usec = (THREADWAIT%1000)*1000;
	const int BUFSIZE = 0xFFFF;
	char buffer[BUFSIZE];
	char * pchStart;
	int indexStart;
	int len;
	int nRecvCount;
	int nReadyCount;
	unsigned int i = 1;
	while (!pThis->m_fExit)
	{
		FD_ZERO(&rfds);
		::EnterCriticalSection(&pThis->m_csBlock);
		for (i = 0; i < pThis->m_vSocketAddr.size(); i++)
		{
			FD_SET(pThis->m_vSocketAddr[i].s,&rfds);
		}
		::LeaveCriticalSection(&pThis->m_csBlock);
		//t.tv_sec = THREADWAIT/1000;						//Windows will not alter the timeval
		//t.tv_usec = (THREADWAIT%1000)*1000;
		if ((nReadyCount = select(pThis->m_SocketMax+1,&rfds,NULL,NULL,&t)) > 0)
		{
			::EnterCriticalSection(&pThis->m_csBlock);
			if (FD_ISSET(pThis->m_ListenSocket,&rfds))
			{
				Socket_Addr sock_addr;
				sock_addr.addrlen = sizeof(struct sockaddr_in);
				sock_addr.s = accept(pThis->m_ListenSocket,(struct sockaddr *)&sock_addr.addr,&sock_addr.addrlen);
				if (sock_addr.s != INVALID_SOCKET)
				{
					if (pThis->m_setBlock.find(((struct sockaddr_in *)&(sock_addr.addr))->sin_addr.s_addr)
						!= pThis->m_setBlock.end() || pThis->m_vSocketAddr.size() >= FD_SETSIZE)
					{
						closesocket(sock_addr.s);
						if (pThis->m_pMsgQueue)
						{
							ConnMsg msg;
							msg.StringID = IDS_REFUSECONNECT;
							msg.iData = 0;
							strncpy_s(msg.szData,
								::inet_ntoa(((struct sockaddr_in *)&(sock_addr.addr))->sin_addr),
								20);
							pThis->m_pMsgQueue->Put(msg);
						}
					}	
					else
					{
						if (sock_addr.s > pThis->m_SocketMax)
						{
							pThis->m_SocketMax = sock_addr.s;
						}
						pThis->m_vSocketAddr.push_back(sock_addr);
#ifdef _DEBUG
						::OutputDebugString(_T("ac "));
#endif
						if (pThis->m_pMsgQueue)
						{
							ConnMsg msg;
							msg.StringID = IDS_CONNECTESTABLISH;
							msg.iData = 0;
							strncpy_s(msg.szData,
								::inet_ntoa(((struct sockaddr_in *)&(sock_addr.addr))->sin_addr),
								20);
							pThis->m_pMsgQueue->Put(msg);
						}

					}
				}
				nReadyCount --;
			}	//end of  'if (FD_ISSET(pThis->m_ListenSocket,&rfds))
			std::vector<Socket_Addr>::iterator iter = pThis->m_vSocketAddr.begin();
			iter++;
			while (nReadyCount && iter != pThis->m_vSocketAddr.end())
			{
				if (FD_ISSET(iter->s,&rfds))
				{
					nReadyCount--;
					nRecvCount = recv(iter->s,buffer,BUFSIZE-1,0);
					if (nRecvCount > 0)
					{
#ifdef _DEBUG
						::OutputDebugString(_T("rc "));
#endif
						buffer[nRecvCount] = 0;
						pchStart = buffer;
						indexStart = 0;
						while (indexStart < nRecvCount)
						{
							iter->szRecvedBuffer += pchStart;
							len = strlen(pchStart);
							if (indexStart + len < nRecvCount)
							{
								::EnterCriticalSection(&pThis->m_csFrame);
								pThis->m_dqRecvedFrame.push_back(iter->szRecvedBuffer);
								::LeaveCriticalSection(&pThis->m_csFrame);
								::ReleaseSemaphore(pThis->m_hSemophere,1,NULL);
								iter->szRecvedBuffer = "";
							}
							pchStart += len + 1;
							indexStart += len + 1;
						}						

					}
					else if (!nRecvCount)
					{
						closesocket(iter->s);
#ifdef _DEBUG
						::OutputDebugString(_T("dc "));
#endif
						if (pThis->m_pMsgQueue)
						{
							ConnMsg msg;
							msg.StringID = IDS_DISCONNECT;
							msg.iData = 0;
							strncpy_s(msg.szData,
								::inet_ntoa(((struct sockaddr_in *)&(iter->addr))->sin_addr),
								20);
							pThis->m_pMsgQueue->Put(msg);
						}
						iter = pThis->m_vSocketAddr.erase(iter);
						continue;
					}
					else 
					{
						closesocket(iter->s);
#ifdef _DEBUG
						::OutputDebugString(_T("fc "));
#endif
						if (pThis->m_pMsgQueue)
						{
							ConnMsg msg;
							msg.StringID = IDS_LOSTCONNECT;
							msg.iData = 0;
							strncpy_s(msg.szData,
								::inet_ntoa(((struct sockaddr_in *)&(iter->addr))->sin_addr),
								20);
							pThis->m_pMsgQueue->Put(msg);
						}
						iter = pThis->m_vSocketAddr.erase(iter);
						continue;
					}
				}  // end of 'if (FD_ISSET(iter->s,&rfds))
				iter++;
			}// end of 'while (nReadyCount && iter != pThis->m_vSocketAddr.end())
			::LeaveCriticalSection(&pThis->m_csBlock);
		}// end of 'if ((nReadyCount = select(pThis->m_SocketMax+1,&rfds,NULL,NULL,&t)) > 0)
		else
		{
#ifdef _DEBUG
			::OutputDebugString(_T("px "));
#endif
		}
	}
	return 0;
}

int CRecvFrame::InitSocket()
{
	Socket_Addr sock_addr;
	for (unsigned i = 0; i < m_vSocketAddr.size(); i++)
	{
		closesocket(m_vSocketAddr[i].s);
	}
	m_vSocketAddr.clear();
	sock_addr.s = socket(AF_UNSPEC,SOCK_STREAM,IPPROTO_TCP);
	if (sock_addr.s == INVALID_SOCKET)
	{
		return -1;
	}
	sock_addr.addrlen = sizeof(struct sockaddr_in);
	((struct sockaddr_in *)&(sock_addr.addr))->sin_port = htons(m_sPort);
	((struct sockaddr_in *)&(sock_addr.addr))->sin_family = AF_INET;
	((struct sockaddr_in *)&(sock_addr.addr))->sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(sock_addr.s,(struct sockaddr *)&sock_addr.addr,sock_addr.addrlen))
	{
		closesocket(sock_addr.s);
		return -1;
	}
	if (listen(sock_addr.s,SOMAXCONN))
	{
		closesocket(sock_addr.s);
		return -1;
	}
	m_vSocketAddr.push_back(sock_addr);
	m_ListenSocket = sock_addr.s;
	m_SocketMax = sock_addr.s;
	return 0;
}