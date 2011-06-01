#ifndef BEACON_H_
#define BEACON_H_

#include "MyThread.h"

class CBeacon : public IMyThread
{
public:
	CBeacon();
	virtual ~CBeacon();
	void SetSelfPort(unsigned short port);		//The self port is only used in passive mode to wait for beacon inquiry
												//It will not affect the current running passive beacon.

	void SetDestPort(unsigned short port);		//The dest port is used in both modes to sendto. 
												//Use PORT_REPLY in passive mode to send beacon information to inquiry source

	unsigned short GetSelfPort() const;	
	unsigned short GetDestPort() const;

	int GetMode() const;
	
	virtual int Start();						//Start as last time.
	virtual int Start(int mode);				//Start the beacon in the specified mode. Socket config is finished here. 
												//If the beacon has already started, it will stop first and start again. 
		
	int SetAdditionData(void * data, int cbCount);	//Additional Data, cbCount==0 for none addition.
	static const int PORT_REPLY = 0;			
	static const int MAX_ADDTIONDATA = 0xFFF0;
	enum Mode{STOP = 0,PASSIVE,ACTIVE};
private:	
	static unsigned int __stdcall sPassiveThreadFunc(void * p);
	static unsigned int __stdcall sActiveThreadFunc(void * p);
	static char m_szBeaconData[];
	int InitSocket();
	unsigned short m_sSelfPort;
	unsigned short m_sDestPort;
	int m_iMode;
	SOCKET m_socket;
	void * m_pSendBuffer;
	int m_cbAddDataCount;
	CRITICAL_SECTION m_cs;
};

#endif

