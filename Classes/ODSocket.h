#ifndef _ODSOCKET_H_
#define _ODSOCKET_H_
#define DEFAULTPORT 9000
#define PORTA 9100
#define BUFFERSIZE 400
#include<string>
#include<vector>
#include<iostream>
#ifdef WIN32
#include <winsock.h>
#pragma comment(lib, "Ws2_32.lib")//?
typedef int socklen_t;
#else
//#include "cocos2d.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include<errno.h>
typedef int	SOCKET;
//#pragma region define win32 const variable in linux
#define INVALID_SOCKET	-1
#define SOCKET_ERROR	-1
//#pragma endregion
#endif
struct Connection
{
	SOCKET hSocket;
	char recvBuffer[BUFFERSIZE];
	char sendBuffer[BUFFERSIZE];
	int recvBytes;
	int sendBytes;
	Connection(SOCKET socket) :hSocket(socket), recvBytes(0), sendBytes(0) {}
};
typedef std::vector<Connection*> ConnectionList;
class ODSocket {
public:
	ODSocket(SOCKET sock = INVALID_SOCKET);
	~ODSocket();
	// Create socket object for snd/recv data
	bool Create(int af, int type, int protocol = 0);
	// Connect socket
	bool Connect(const char* ip, unsigned short port);
	//#region server
	// Bind socket
	bool Bind(unsigned short port);
	// Listen socket
	bool Listen(int backlog = 5);
	// Accept socket
	bool Accept(ODSocket& s, char* fromip = NULL);
	//#endregion
	// Send socket
	int Send(const char* buf, int len, int flags = 0);
	// Recv socket
	int Recv(char* buf, int len, int flags = 0);
	int SendTo(char* buf, int len, int flags, char*, int port);
	int RecvFrom(char* buf, int len, int flags, std::string& from, int& port);
	bool setBroadcast()
	{
		socklen_t opt = 1;
		if (setsockopt(m_sock, SOL_SOCKET, SO_BROADCAST, (char*)&opt, sizeof(opt)) < 0)
			return false;
		return true;
	}
	// Close socket
	int Close();
	// Get errno
	int GetError();
	//#pragma region just for win32
	// Init winsock DLL 
	static int Init();
	// Clean winsock DLL
	static int Clean();
	//#pragma endregion
	// Domain parse
	static bool DnsParse(const char* domain, char* ip);
	ODSocket& operator = (SOCKET s);
    operator SOCKET ();
protected:
	SOCKET m_sock;
};
#endif