#ifndef EMINPUT_H_
#define EMINPUT_H_

#include "Beacon.h"
#include "RecvFrame.h"
#include "MyThread.h"
#include "InputFilter.h"
#include "SafeQueue.h"

class CEmInput : public IMyThread
{
public:
	CEmInput();
	virtual ~CEmInput();
	void SetBeaconPort(unsigned short port);	//Set Beacon self port. EmInput will use passive beacon mode
	void SetRecvFramePort(unsigned short port);
	void SetInputFilter(CInputFilter * filter);
	void SetMsgQueue(CSafeQueue<ConnMsg> * pQueue);
	virtual int Start();	
	virtual int Stop();							//Start to receive message, wait for beacon search, and emulate inputs
private:
	static unsigned int __stdcall sThreadFunc(void * p);
	CBeacon m_beacon;
	CRecvFrame m_recvframe;
	CInputFilter * m_pInputFilter;
	CRITICAL_SECTION m_cs;
};

#endif