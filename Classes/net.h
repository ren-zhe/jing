#ifndef __NET_SCENE_H__
#define __NET_SCENE_H__
#include "ODSocket.h"
#include<thread>
#include<mutex>

class Net 
{
public:
	bool makeServer();
	bool makeClient();
	void clientRecv(std::string temp);
	//virtual void Manage(float dt);
	void accept();
	void SendBroadcast();
	void RecvBroadcast();
	void ResetFDSet(fd_set& fdRead, fd_set& fdWrite, fd_set& fdExcept, SOCKET sdListen, const ConnectionList& conns);
	int CheckAccept(const fd_set& fdRead, const fd_set& fdExcept, SOCKET sdListen, ConnectionList& conns);
	void CheckConn(const fd_set& fdRead, const fd_set& fdWrite, const fd_set& fdExcept, ConnectionList& conns);
	bool TryRead(Connection* pConn);
	bool TryWrite(Connection* pConn);
	bool PassiveShutdown(SOCKET sd, const char* buff, int len);
	bool createUdpSocket();
	static Net* getInstance()
	{
		if (m_net == NULL)
		{
			m_net = new Net();
		}
		return m_net;
	}
    void destroyInstance()
	{

		if (connectSocket)
		{
			delete connectSocket;
			connectSocket = NULL;
		}
		if (listenSocket)
		{
			delete listenSocket;
			listenSocket = NULL;
		}
		for (std::vector<Connection*>::iterator it = clientList.begin(); it != clientList.end(); it++)
		{
			if (*it)
			{
				delete *it;
				*it = NULL;
			}
		}
	}
	void DeleteSocket();
	ODSocket* connectSocket;
	ODSocket* listenSocket;
	ODSocket* udpSocket;
	ConnectionList clientList;
	std::string serverIP;
	int serverPort;
	std::vector<std::string> str;
	std::mutex m_mutex;
	std::mutex m_mutex1;
	bool connect;
	static Net* m_net;
	int maxfd;
	bool exit;
private:
	Net() :listenSocket(NULL), udpSocket(NULL), connectSocket(NULL), connect(false), maxfd(-1)
	{
		clientList.clear();
		serverIP.clear();
		exit = false;
	}
};

#endif // __NET_SCENE_H__

