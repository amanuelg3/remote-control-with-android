#ifndef MYTHREAD_H_
#define MYTHREAD_H_

class IMyThread
{
public:
	IMyThread();
	virtual ~IMyThread();								//Be careful!! 
														//Subclass need call Stop() Explicitly in its dtor, before releasing its synchronization kernel objects.
														//Stop() in this dtor should never be used
	virtual int Start() = 0;
	virtual int Stop();									//Stop thread, including RaiseExit()
	virtual void RaiseExit();							//Inform the thread to exit, Stop() must be called to wait thread exit
protected:
	static const long THREADWAIT = 2000;				//IMyThread will wait 2*THREADWAIT milliseconds before stop the thread forcely. 
	unsigned int (__stdcall * m_ThreadAddr)(void *);
	bool m_fExit;										//Thread should exit when m_fExit == true
	HANDLE m_hThread;
};


#endif