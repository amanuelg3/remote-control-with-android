#include "stdafx.h"
#include "EmInput.h"
#include "FrameCnvt.h"

CEmInput::CEmInput()
{
	m_pInputFilter = NULL;
	this->m_ThreadAddr = sThreadFunc;
	::InitializeCriticalSection(&m_cs);
}
	
CEmInput::~CEmInput()
{
	Stop();
	::DeleteCriticalSection(&m_cs);
}

void CEmInput::SetBeaconPort(unsigned short port)
{
	m_beacon.SetSelfPort(port);
}

void CEmInput::SetRecvFramePort(unsigned short port)
{
	m_recvframe.SetSockPort(port);
}

void CEmInput::SetMsgQueue(CSafeQueue<ConnMsg> * pQueue)
{
	m_recvframe.SetMsgQueue(pQueue);
}

void CEmInput::SetInputFilter(CInputFilter * filter)
{
	::EnterCriticalSection(&m_cs);
	m_pInputFilter = filter;
	::LeaveCriticalSection(&m_cs);
}

int CEmInput::Start()
{
	int rtVal;
	if (rtVal = m_recvframe.Start())
	{
#ifdef _DEBUG
		::OutputDebugString(_T("CEmInput: RecvFrame Start Error"));
#endif		
		return rtVal;
	}

	const int HNAMELEN = 256;
	char szHostName[HNAMELEN];
	if (!::gethostname(szHostName,HNAMELEN))
	{
		m_beacon.SetAdditionData(szHostName,strlen(szHostName)+1);
	}

	if (rtVal = m_beacon.Start(CBeacon::PASSIVE))
	{
#ifdef _DEBUG
		::OutputDebugString(_T("CEmInput: Beacon Start Error"));
#endif		
		return rtVal;
	}
	return IMyThread::Start();
}

int CEmInput::Stop()
{
	int rtVal = 0;

	this->RaiseExit();
	m_beacon.RaiseExit();
	m_recvframe.RaiseExit();

	rtVal |=  m_beacon.Stop();
	rtVal |= m_recvframe.Stop();
	rtVal |= IMyThread::Stop();

	return rtVal;
}

unsigned int __stdcall CEmInput::sThreadFunc(void * p)
{
	CEmInput * pThis = dynamic_cast<CEmInput *>(reinterpret_cast<IMyThread *>(p));
	if (!pThis)
	{
#ifdef _DEBUG
		::OutputDebugString(_T("CEmInput: Thread this Pointer Error"));
#endif
		return -1;
	}
	HANDLE hSemophere = pThis->m_recvframe.GetSemophere();
	CFrameCnvt * pCnvt = new CFrameCnvt();
	std::string frame;
	int len;
	std::vector<INPUT> * pInputs;
	while (!pThis->m_fExit)
	{
		if (::WaitForSingleObject(hSemophere,THREADWAIT) == WAIT_OBJECT_0)
		{
			frame = pThis->m_recvframe.GetFrame();
#ifdef _DEBUG
			DWORD dw =::GetTickCount();
			TCHAR bf[256];
			_itot_s(dw,bf,256,10);
			OutputDebugString(bf);
			OutputDebugString(_T("\n===========Start==========\n"));
			OutputDebugStringA(frame.c_str());
			OutputDebugString(_T("\n============End=========\n"));
#endif
			len = frame.length();
			while (len > 0 && frame[len-1] != '>')
			{
				len--;
			}
			frame.resize(len);
			pCnvt->Reset();
			pCnvt->Convert(frame.c_str(),frame.length()+1);
			pInputs = (std::vector<INPUT> *)&pCnvt->GetResult();
			::EnterCriticalSection(&pThis->m_cs);
			if (pThis->m_pInputFilter)
			{
				pThis->m_pInputFilter->ProcessData(*pInputs);
			}
			::LeaveCriticalSection(&pThis->m_cs);
			if (pInputs->size() != 0)
			{
				SendInput(pInputs->size(),&(*pInputs)[0],sizeof(INPUT));
			}
		}
	}
	delete pCnvt;
	return 0;
}

