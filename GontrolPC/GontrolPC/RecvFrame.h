#ifndef RECVFRAME_H_
#define RECVFRAME_H_
#include "MyThread.h"
#include "SafeQueue.h"
class CRecvFrame : public IMyThread
{
public:
	CRecvFrame();
	virtual ~CRecvFrame();
	void SetSockPort(unsigned short Port);
	unsigned short GetSockPort() const;
	void SetMsgQueue(CSafeQueue<ConnMsg> * pQueue);
	HANDLE GetSemophere() const;
	void AddBlockAddr(const char * szAddr);
	void DelBlockAddr(const char * szAddr);
	virtual int Start();
	std::string GetFrame();
private:
	static unsigned int __stdcall sThreadFunc(void * p);
	int InitSocket();
	unsigned short m_sPort;
	struct Socket_Addr
	{
		SOCKET s;
		struct sockaddr_storage addr;
		int addrlen;
		std::string szRecvedBuffer;
	};
	HANDLE m_hSemophere;
	CRITICAL_SECTION m_csFrame;
	CRITICAL_SECTION m_csBlock;
	SOCKET m_ListenSocket;
	SOCKET m_SocketMax;
	std::set<u_long> m_setBlock;
	std::vector<Socket_Addr> m_vSocketAddr;
	std::deque<std::string> m_dqRecvedFrame;
	CSafeQueue<ConnMsg> * m_pMsgQueue;
};

#endif