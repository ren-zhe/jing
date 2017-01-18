#include "net.h"
#include<iostream>
#include<thread>
Net* Net::m_net = NULL;
bool Net::makeServer()
{
	if (connectSocket != NULL)
		return false;
	if (listenSocket)
		return false;
	listenSocket = new ODSocket();
	if (!listenSocket)
	{
		//OutputDebugString(L"new error\n");
		return false;
	}
	if (listenSocket->Init() == -1)
	{
		//OutputDebugString(L"Init error\n");
		return false;
	}
	if (!listenSocket->Create(AF_INET, SOCK_STREAM, 0))
	{
		//OutputDebugString(L"Create listensocket error\n");
		return false;
	}
	int port = DEFAULTPORT;
	if (!listenSocket->Bind(port))
	{
		//OutputDebugString(L"Bind error\n");
		return false;
	}
	if (!listenSocket->Listen())
	{
		//OutputDebugString(L"Listen error\n");
		return false;
	}
	std::thread acceptThread(&Net::accept, this);
	acceptThread.detach();
	std::thread broadcastThread(&Net::SendBroadcast, this);
	broadcastThread.detach();
	return true;
}
bool Net::makeClient()
{
	if (serverIP.empty())
		return false;
	if (connectSocket != NULL)
		return false;
	connectSocket = new ODSocket();
	if (connectSocket == NULL)
	{
		//OutputDebugString(L"new error\n");
		return false;
	}
	if (connectSocket->Init() == -1)
	{
		//OutputDebugString(L"Init error\n");
		return false;
	}
	if (!connectSocket->Create(AF_INET, SOCK_STREAM, 0))
	{
		//OutputDebugString(L"Create error\n");
		return false;
	}
	/*if (!connectSocket->Connect(temp.c_str(), DEFAULTPORT))
	{
	log("Connect error1");
	return;
	}*/
	std::string temp = serverIP;
	//thread
	std::thread rr(&Net::clientRecv, this, temp);
	rr.detach();
	return true;
}
void Net::clientRecv(std::string temp)
{
	while (!connect)
	{
		if (Net::getInstance()->exit)
			return;
		if (connectSocket->Connect(temp.c_str(), DEFAULTPORT))
		{
			connect = true;
			//	OutputDebugString(L"connect ok\n");
		}
	}
	fd_set fdRead, fdWrite, fdExcept;
	SOCKET sd = *connectSocket;
	clientList.push_back(new Connection(sd));
	if (sd > maxfd)
		maxfd = sd;
	while (true)
	{
		if (Net::getInstance()->exit)
			return;
		if (!connectSocket)
			break;
		/*	OutputDebugString(L"circle\n");*/
		FD_ZERO(&fdRead);
		FD_ZERO(&fdWrite);
		FD_ZERO(&fdExcept);
		ConnectionList::const_iterator it = clientList.begin();
		if ((*it)->recvBytes < BUFFERSIZE)
		{
			FD_SET((*it)->hSocket, &fdRead);
		}
		if ((*it)->sendBytes > 0)
		{
			FD_SET((*it)->hSocket, &fdWrite);
		}
		FD_SET((*it)->hSocket, &fdExcept);
		timeval time{ 0, 300 };
#ifdef WIN32
		int ret = select(0, &fdRead, &fdWrite, &fdExcept, &time);
#else   
		int ret = select(maxfd + 1, &fdRead, &fdWrite, &fdExcept, &time);
#endif
		/*OutputDebugString(L"after select\n");*/
		if (ret < 0)
		{
			//OutputDebugString(L"select error\n");
			//std::cout << "select error" << WSAGetLastError() << std::endl;
			break;
		}
		else if (ret == 0)
			continue;
		CheckConn(fdRead, fdWrite, fdExcept, clientList);
	}
}
void Net::accept()
{
	fd_set fdRead, fdWrite, fdExcept;
	SOCKET sdListen = *listenSocket;
	if (maxfd < sdListen)
		maxfd = sdListen;
	while (true)
	{
		if (Net::getInstance()->exit)
			return;
		if (!listenSocket)
		{
			//OutputDebugStringA("accept quit");
			break;
		}
		/*if (clean)
		{
		closesocket(sdListen);
		}*/
		ResetFDSet(fdRead, fdWrite, fdExcept, sdListen, clientList);
		timeval time = { 0,300 };
#ifdef WIN32
		int ret = select(0, &fdRead, &fdWrite, &fdExcept, &time);
#else
		int ret = select(maxfd + 1, &fdRead, &fdWrite, &fdExcept, &time);
#endif
		if (ret < 0)
		{
			//OutputDebugString(L"select error\n");
			//	std::cout << "select error" << WSAGetLastError() << std::endl;
			break;
		}
		else if (ret == 0)
			continue;
		ret = CheckAccept(fdRead, fdExcept, sdListen, clientList);
		if (ret == SOCKET_ERROR)
		{
			//OutputDebugString(L"checkaccept error\n");
		break;
		}
		CheckConn(fdRead, fdWrite, fdExcept, clientList);
	}
	/*connectSocket = new ODSocket();
	if (!connectSocket)
	{
	log("new error");
	return;
	}
	if (connectSocket->Init() == -1)
	{

	log("Init error");

	return;

	}

	if (!connectSocket->Create(AF_INET, SOCK_STREAM, 0))

	{

	log("Create error");

	return;

	}

	if (!listenSocket->Accept(*connectSocket))

	{

	log("Accept error.");

	return;

	}*/

}
void Net::SendBroadcast()
{
	if (!udpSocket)
		return;
	char* ip = NULL;
	while (1)
	{
		if (Net::getInstance()->exit)
			return;
		if (connect)
			break;
		udpSocket->SendTo("999", sizeof("999"), 0, ip, PORTA);
		OutputDebugString(L"send999\n");
#ifdef WIN32 
		Sleep(300);
#else
		usleep(300);
#endif
	}
}
void Net::RecvBroadcast()
{
	while (1)
	{
		//if (!serverIP.empty())//
		//{
		//	//OutputDebugString(L"connect ip\n");
		//	break;
		//}
		if (Net::getInstance()->exit)
			return;
		char buf[BUFFERSIZE];
		std::string temp;
		int port;
		int len = udpSocket->RecvFrom(buf, BUFFERSIZE, 0, temp, port);
		//log(Value(len).asString().c_str());
		if (len >= 3)
		{
			if (buf[0] == '9' && buf[1] == '9' && buf[2] == '9')
			{
				OutputDebugStringA("recv999");
				//	log(serverIP.c_str());
				if (serverIP.empty())
				{
					m_mutex.lock();
					str.push_back("999");
					m_mutex.unlock();
					OutputDebugStringA("str recv999");
					serverIP = temp;
					serverPort = port;
				}
			}
		}
	}
}
void Net::ResetFDSet(fd_set & fdRead, fd_set & fdWrite, fd_set & fdExcept, SOCKET sdListen, const ConnectionList & conns)
{
	FD_ZERO(&fdRead);
	FD_ZERO(&fdWrite);
	FD_ZERO(&fdExcept);
	FD_SET(sdListen, &fdRead);
	FD_SET(sdListen, &fdExcept);
	ConnectionList::const_iterator it = conns.begin();
	//m_mutex1.lock();
	for (; it != conns.end(); ++it)
	{
		Connection* pConn = *it;
		if (pConn->recvBytes < BUFFERSIZE)
		{
			FD_SET(pConn->hSocket, &fdRead);
		}
		if (pConn->sendBytes > 0)
		{
			FD_SET(pConn->hSocket, &fdWrite);
		}
		FD_SET(pConn->hSocket, &fdExcept);
	}
	/*m_mutex1.unlock();*/
}
int Net::CheckAccept(const fd_set & fdRead, const fd_set & fdExcept, SOCKET sdListen, ConnectionList & conns)
{
	int lastErr = 0;
	if (FD_ISSET(sdListen, &fdExcept))//sdListen 异常
	{
		socklen_t errlen = sizeof(lastErr);
		getsockopt(sdListen, SOL_SOCKET, SO_ERROR, (char*)&lastErr, &errlen);
		//OutputDebugString(L"I/O error\n");
		//log(Value(lastErr).asString().c_str());
		std::cout << "I/O error" << lastErr << std::endl;
		return SOCKET_ERROR;
	}
	if (FD_ISSET(sdListen, &fdRead))//可以accept
	{
		//OutputDebugString(L"accept ok\n");
		if (conns.size() >= FD_SETSIZE - 1)
		{
			return 0;
		}
		sockaddr_in client;
		socklen_t size = sizeof(sockaddr_in);
		SOCKET sd = ::accept(sdListen, (sockaddr*)&client, &size);
		//lastErr = WSAGetLastError();
		if (sd == INVALID_SOCKET)//&& lastErr != WSAEWOULDBLOCK
		{
			//OutputDebugString(L"accept error\n");
			//std::cout << "accept error" << lastErr << std::endl;
			return SOCKET_ERROR;
		}
		if (sd != INVALID_SOCKET)
		{
			connect = true;
			if (maxfd < sd)
				maxfd = sd;
#ifdef WIN32
			u_long noBlock = 1;
		    if (ioctlsocket(sd, FIONBIO, &noBlock) == SOCKET_ERROR)
			{
				OutputDebugString(L"ioctlsocket error\n");
				std::cout << "ioctlsocket error" << WSAGetLastError() << std::endl;
				return SOCKET_ERROR;
			}
#else
			int flags = fcntl(sd, F_GETFL, 0);                        //获取文件的flags值。
			fcntl(sd, F_SETFL, flags | O_NONBLOCK);   //设置成非阻塞模式；
#endif
      		conns.push_back(new Connection(sd));
		}
	}
	return 0;
}
void Net::CheckConn(const fd_set & fdRead, const fd_set & fdWrite, const fd_set & fdExcept, ConnectionList & conns)
{
	//OutputDebugString(L"chekconn\n");
	ConnectionList::iterator it = conns.begin();
	while (it != conns.end())
	{
		Connection* pConn = *it;
		bool ok = true;
		if (FD_ISSET(pConn->hSocket, &fdExcept))
		{
			ok = false;
			int lastErr;
			socklen_t errlen = sizeof(lastErr);
			getsockopt(pConn->hSocket, SOL_SOCKET, SO_ERROR, (char*)&lastErr, &errlen);
			//OutputDebugString(L"I/O error\n");
			std::cout << "I/O error" << lastErr << std::endl;
		}
		else
		{
			if (FD_ISSET(pConn->hSocket, &fdRead))
			{
				ok = TryRead(pConn);
			}
			else if (FD_ISSET(pConn->hSocket, &fdWrite))
			{
				ok = TryWrite(pConn);
			}
		}
		if (!ok)
		{
			//closesocket(pConn->hSocket);
			delete pConn;
			it = conns.erase(it);
		}
		else
			++it;
	}
}
bool Net::TryRead(Connection * pConn)
{
	//OutputDebugString(L"read enter\n");
	int ret = recv(pConn->hSocket, pConn->recvBuffer + pConn->recvBytes, BUFFERSIZE - pConn->recvBytes, 0);
	if (ret > 0)
	{
		//OutputDebugString(L"read words;\n");
		//OutputDebugStringA(pConn->recvBuffer);
		pConn->recvBytes += ret;
		m_mutex.lock();
		str.push_back(pConn->recvBuffer);
		m_mutex.unlock();
		pConn->recvBytes -= ret;
		//if (pConn->recvBytes >= 4)
		//{
		//	log("recv 4words");

		//	if (pConn->recvBuffer[0] == '1')//合法消息

		//	{

		//		if (pConn->recvBuffer[1] == '3' && pConn->recvBuffer[2] == '3')//first消息

		//		{

		//			/*myTurn = false;

		//			flag = tag_x;*/

		//			log("recv first");

		//			m_mutex.lock();

		//			str.push_back(pConn->recvBuffer);

		//			m_mutex.unlock();

		//		}

		//		else if (pConn->recvBuffer[2] >= '0' && pConn->recvBuffer[2] < '3' && pConn->recvBuffer[1] >= '0' && pConn->recvBuffer[1] < '3')//对方下棋子消息

		//		{

		//			myTurn = true;

		//			m_mutex.lock();

		//			str.push_back(pConn->recvBuffer);

		//			m_mutex.unlock();

		//			auto nextFlag = flag == tag_o ? tag_x : tag_o;//获取对方棋子类型

		//			data[pConn->recvBuffer[1] - '0'][pConn->recvBuffer[2] - '0'] = nextFlag;

		//			log("recv keys");

		//		}

		//		pConn->recvBytes -= 4;

		//		memmove(pConn->recvBuffer, pConn->recvBuffer + 4, pConn->recvBytes);

		//	}

		//}
		return true;
	}
	else if (ret == 0)
	{
		connect = false;
		//OutputDebugString(L"connection close by peer\n");
		std::cout << "connection close by peer" << std::endl;
		PassiveShutdown(pConn->hSocket, pConn->recvBuffer, pConn->recvBytes);
		return false;
	}
	else
	{
		/*int lastErr = WSAGetLastError();

		if (lastErr == WSAEWOULDBLOCK)

		{

		return true;

		}

		OutputDebugString(L"recv error\n");

		std::cout << "recv error" << lastErr << std::endl;*/
	return false;
	}
}
bool Net::TryWrite(Connection * pConn)
{
	//OutputDebugString(L"write enter\n");
	//m_mutex.lock();
	int ret = send(pConn->hSocket, pConn->sendBuffer, pConn->sendBytes, 0);
	if (ret > 0)
	{
		//OutputDebugString(L"write words;\n");
		pConn->sendBytes -= ret;
		/*if (pConn->sendBytes > 0)
		{
		memmove(pConn->sendBuffer, pConn->sendBuffer + ret, pConn->sendBytes);

		}

		else

		{

		pConn->sendBuffer[0] = '\0';

		}*/
		return true;
	}
	else if (ret == 0)
	{
		connect = false;
		//OutputDebugString(L"connection close by peer\n");
		std::cout << "connection close by peer" << std::endl;
		PassiveShutdown(pConn->hSocket, pConn->sendBuffer, pConn->sendBytes);
		return false;
	}
	else
	{
		/*int lastErr = WSAGetLastError();

		if (lastErr == WSAEWOULDBLOCK)

		{

		return true;

		}

		OutputDebugString(L"send error\n");

		std::cout << "send error" << lastErr << std::endl;*/
		return false;
	}
}
bool Net::PassiveShutdown(SOCKET sd, const char * buff, int len)
{
	if (buff != NULL && len > 0)
	{
#ifdef WIN32
		u_long noBlock = 0;
		if (ioctlsocket(sd, FIONBIO, &noBlock) == SOCKET_ERROR)
		{
			OutputDebugString(L"ioctlsocket error\n");
			std::cout << "ioctlsocket error" << WSAGetLastError() << std::endl;
			return false;
		}
#else
		int flags = fcntl(sd, F_GETFL, 0);                        //获取文件的flags值。
		fcntl(sd, F_SETFL, flags | O_NONBLOCK);   //设置成非阻塞模式；
#endif
		int nSend = 0;
		while (nSend < len)
		{
			int temp = send(sd, &buff[nSend], len - nSend, 0);
			if (temp > 0)
			{
				nSend += temp;
			}
			else if (temp == SOCKET_ERROR)
			{
				//OutputDebugString(L"send error\n");
				//	std::cout << "send error" << WSAGetLastError() << std::endl;
				return false;
			}
			else
			{
				//OutputDebugString(L"Connection closed unexceptedly by peer\n");
				std::cout << "Connection closed unexceptedly by peer" << std::endl;
				break;
			}
		}
	}
	if (shutdown(sd, 1) == SOCKET_ERROR)
	{
		//OutputDebugString(L"shutdown error\n");
		//std::cout << "shutdown error" << WSAGetLastError() << std::endl;
		return false;
	}
	return true;
}
bool Net::createUdpSocket()
{
	if (udpSocket == NULL)
	{
		udpSocket = new ODSocket();
		if (udpSocket == NULL)
		{
			//OutputDebugString(L"udp create error\n");
			return false;
		}
		if (udpSocket->Init() == -1)
		{
			//OutputDebugString(L"udp init error\n");
			return false;
		}
		if (!udpSocket->Create(AF_INET, SOCK_DGRAM, 0))
		{
			//OutputDebugString(L"create udp error\n");
			return false;
		}
		if (!udpSocket->Bind(PORTA))
		{
		//OutputDebugString(L"bind error\n");
			return false;
		}
		if (!udpSocket->setBroadcast())
		{
			//OutputDebugString(L"set broadcast error\n");
			return false;
		}
	}
	return true;//cun zai huozhe bucunzai
}
void Net::DeleteSocket()
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
	clientList.clear();
	connect = false;
	serverIP.clear();
	str.clear();
	maxfd = -1;
	exit = false;
}
