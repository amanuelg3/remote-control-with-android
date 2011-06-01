#include "stdafx.h"
#include "Beacon.h"

char CBeacon::m_szBeaconData[] = {"BEACONxBEACON"};
CBeacon::CBeacon()
{
	m_sSelfPort = 0;
	m_sDestPort = PORT_REPLY;
	m_iMode = STOP;
	m_pSendBuffer = NULL;
	m_cbAddDataCount = 0;
	m_socket = INVALID_SOCKET;
	::InitializeCriticalSection(&m_cs);
}

CBeacon::~CBeacon()
{
	Stop();
	if (m_pSendBuffer)
	{
		SetAdditionData(NULL,0);
	}
	if (m_socket != INVALID_SOCKET)
	{
		closesocket(m_socket);
	}
	::DeleteCriticalSection(&m_cs);
}

void CBeacon::SetSelfPort(unsigned short port)
{
	m_sSelfPort = port;
}

void CBeacon::SetDestPort(unsigned short port)
{
	m_sDestPort = port;
}

unsigned short CBeacon::GetSelfPort() const
{
	return m_sSelfPort;
}

unsigned short CBeacon::GetDestPort() const
{
	return m_sDestPort;
}

int CBeacon::GetMode() const
{
	return m_iMode;
}

int CBeacon::Start()
{
	if (InitSocket())
	{
#ifdef _DEBUG
		::OutputDebugString(_T("CBeacon: Init Socket Error"));
#endif
		return -1;
	}
	return IMyThread::Start();
}

int CBeacon::Start(int mode)
{
	m_iMode = mode;
	switch (mode)
	{
	case PASSIVE:
		this->m_ThreadAddr = sPassiveThreadFunc;
		break;
	case ACTIVE:
		this->m_ThreadAddr = sActiveThreadFunc;
		break;
	case STOP:
		this->m_ThreadAddr = NULL;
	}
	return CBeacon::Start();
}

int CBeacon::SetAdditionData(void * data, int cbCount)
{
	if (cbCount > MAX_ADDTIONDATA)
	{
		return 1;
	}
	::EnterCriticalSection(&m_cs);
	if (m_pSendBuffer)
	{
		free(m_pSendBuffer);
	}
	m_cbAddDataCount = cbCount;
	if (m_cbAddDataCount)
	{
		m_pSendBuffer = malloc(m_cbAddDataCount+sizeof(m_szBeaconData));
		if (!m_pSendBuffer)
		{
			::LeaveCriticalSection(&m_cs);
			return -1;
		}
		memcpy(m_pSendBuffer,m_szBeaconData,sizeof(m_szBeaconData));
		memcpy((char *)m_pSendBuffer+sizeof(m_szBeaconData),data,m_cbAddDataCount);
	}
	else
	{
		m_pSendBuffer = 0;
	}
	::LeaveCriticalSection(&m_cs);
	return 0;
}

unsigned int __stdcall CBeacon::sPassiveThreadFunc(void * p)
{
	CBeacon * pThis = dynamic_cast<CBeacon *>(reinterpret_cast<IMyThread *>(p));
	if (!pThis)
	{
#ifdef _DEBUG
		::OutputDebugString(_T("CBeacon: Thread this Pointer Error"));
#endif
		return -1;
	}
	struct fd_set rfds;
	struct timeval t;
	t.tv_sec = THREADWAIT/1000;
	t.tv_usec = (THREADWAIT%1000)*1000;
	const int BUFSIZE = 0xFFFF;
	char buffer[BUFSIZE];
	struct sockaddr_in addrfrom;
	int addrsize;
	while (!pThis->m_fExit)
	{
		FD_ZERO(&rfds);
		FD_SET(pThis->m_socket,&rfds);
		//t.tv_sec = THREADWAIT/1000;						//Windows will not alter the timeval
		//t.tv_usec = (THREADWAIT%1000)*1000;
		if (select(pThis->m_socket+1,&rfds,NULL,NULL,&t) > 0
			&& FD_ISSET(pThis->m_socket,&rfds))
		{
			addrsize = sizeof(addrfrom);
			if (recvfrom(pThis->m_socket,buffer,BUFSIZE,0,(struct sockaddr *)&addrfrom,&addrsize) > 0
				&& !memcmp(buffer,m_szBeaconData,sizeof(m_szBeaconData)))
			{
				printf("BB ");
				if (pThis->m_sDestPort != PORT_REPLY)
				{
					addrfrom.sin_port = htons(pThis->m_sDestPort);
				}
				::EnterCriticalSection(&pThis->m_cs);
				if (pThis->m_pSendBuffer)
				{
					//printf("%s:%d ",inet_ntoa(addrfrom.sin_addr),::ntohs(addrfrom.sin_port));
					sendto(pThis->m_socket,(char*)pThis->m_pSendBuffer,pThis->m_cbAddDataCount+sizeof(m_szBeaconData),
						0,(struct sockaddr *)&addrfrom,sizeof(addrfrom));
				}
				else
				{
					sendto(pThis->m_socket,m_szBeaconData,sizeof(m_szBeaconData),
						0,(struct sockaddr *)&addrfrom,sizeof(addrfrom));
				}
				::LeaveCriticalSection(&pThis->m_cs);
			}
		}
	}
	return 0;
}

unsigned int __stdcall CBeacon::sActiveThreadFunc(void * p)
{
	CBeacon * pThis = dynamic_cast<CBeacon *>(reinterpret_cast<IMyThread *>(p));
	if (!pThis)
	{
#ifdef _DEBUG
		::OutputDebugString(_T("CBeacon: Thread this Pointer Error"));
#endif
		return -1;
	}
	struct sockaddr_in addrbroadcast;
	int sendcount;
	addrbroadcast.sin_family = AF_INET;
	addrbroadcast.sin_port = htons(pThis->m_sDestPort);
	addrbroadcast.sin_addr.s_addr = inet_addr("255.255.255.255");
	while (!pThis->m_fExit)
	{
		::EnterCriticalSection(&pThis->m_cs);
		if (pThis->m_pSendBuffer)
		{
			sendcount = sendto(pThis->m_socket,(char*)pThis->m_pSendBuffer,pThis->m_cbAddDataCount+sizeof(m_szBeaconData),
				0,(struct sockaddr *)&addrbroadcast,sizeof(addrbroadcast));
		}
		else
		{
			sendcount = sendto(pThis->m_socket,m_szBeaconData,sizeof(m_szBeaconData),
				0,(struct sockaddr *)&addrbroadcast,sizeof(addrbroadcast));
		}
		::LeaveCriticalSection(&pThis->m_cs);
		if (sendcount >= 0)
		{
			printf("%d Byte Sent\n",sendcount);
		}
		::Sleep(THREADWAIT/2);
	}
	return 0;
}

int CBeacon::InitSocket()
{
	SOCKET s = socket(AF_UNSPEC,SOCK_DGRAM,IPPROTO_UDP);
	if (s == INVALID_SOCKET)
	{
		return -1;
	}
	int reuseaddr = 1,broadcast = 1;
	if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&reuseaddr, sizeof(reuseaddr))) 
	{
		closesocket(s);
		return -1;
	}
	if (setsockopt(s, SOL_SOCKET, SO_BROADCAST, (char *)&broadcast, sizeof(broadcast))) 
	{
		closesocket(s);
		return -1;
	}
	struct sockaddr_in addr;
	addr.sin_port = htons(m_sSelfPort);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(s,(struct sockaddr *)&addr,sizeof(addr)))
	{
		closesocket(s);
		return -1;
	}
	if (m_socket != INVALID_SOCKET)
	{
		closesocket(m_socket);
	}
	m_socket = s;
	return 0;

}