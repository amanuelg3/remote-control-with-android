#ifndef SAFEQUEUE_H_
#define SAFEQUEUE_H_

class TimeoutException : public std::exception
{

};

template<typename T>
class CSafeQueue
{
public:
	CSafeQueue();
	~CSafeQueue();
	T Take(int WaitTime);
	void Put(const T & elem);
private:
	std::deque<T> m_q;
	HANDLE m_hSemaphore;
	CRITICAL_SECTION m_cs;
};


template<typename T>
CSafeQueue<T>::CSafeQueue()
{
	InitializeCriticalSection(&m_cs);
	m_hSemaphore = CreateSemaphore(NULL,0,0x7FFFFFFF,NULL);
}

template<typename T>
CSafeQueue<T>::~CSafeQueue()
{
	CloseHandle(m_hSemaphore);
	DeleteCriticalSection(&m_cs);
}

template<typename T>
T CSafeQueue<T>::Take(int WaitTime)
{
	T elem;
	if (WaitForSingleObject(m_hSemaphore,WaitTime) == WAIT_OBJECT_0)
	{
		EnterCriticalSection(&m_cs);
		elem = m_q.front();
		m_q.pop_front();
		LeaveCriticalSection(&m_cs);
	}
	else
	{
		throw TimeoutException();
	}
	return elem;
}

template<typename T>	
void CSafeQueue<T>::Put(const T & elem)
{
	EnterCriticalSection(&m_cs);
	m_q.push_back(elem);
	LeaveCriticalSection(&m_cs);
	ReleaseSemaphore(m_hSemaphore,1,NULL);
}

struct ConnMsg
{
	int StringID;
	int iData;
	char szData[128];
};

#endif