#include "stdafx.h"
#include "MyThread.h"

IMyThread::IMyThread()
{
	m_ThreadAddr = NULL;
	m_fExit = true;
	m_hThread = NULL;
}

IMyThread::~IMyThread()
{
	if (m_hThread)
	{
		Stop();
	}
}

int IMyThread::Start()
{
	unsigned int threadid;
	if (!m_ThreadAddr)
	{
		return ERROR_INVALID_PARAMETER;
	}
	if (m_hThread)
	{
		Stop();
	}
	m_fExit = false;
	m_hThread = (HANDLE)_beginthreadex(NULL,0,m_ThreadAddr,this,0,&threadid);
	if (m_hThread)
	{
		return 0;
	}
	else
	{
		return GetLastError();
	}

	return 0;

}

void IMyThread::RaiseExit()
{
	m_fExit = true;
}

int IMyThread::Stop()
{
	int rtVal = 0;
	RaiseExit();
	if (!m_hThread)
	{
		return ERROR_INVALID_PARAMETER;
	}
	if (::WaitForSingleObject(m_hThread,2*THREADWAIT) != WAIT_OBJECT_0)
	{
		::TerminateThread(m_hThread,-1);
		rtVal = -1;
	}
	m_hThread = NULL;
	return rtVal;
}


