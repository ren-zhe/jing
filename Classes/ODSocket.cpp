#include "ODSocket.h"
#include<stdio.h>
//#ifdef WIN32
//#include "StdAfx.h"
//#pragma comment(lib, "wsock32")dd
//#endif
ODSocket::ODSocket(SOCKET sock)
{
	m_sock = sock;
}
ODSocket::~ODSocket()
{
	Close();
	Clean();
}

int ODSocket::Init()
{
#ifdef WIN32
	/*
	http://msdn.microsoft.com/zh-cn/vstudio/ms741563(en-us,VS.85).aspx
	typedef struct WSAData {
	WORD wVersion;								//winsock version
	WORD wHighVersion;							//The highest version of the Windows Sockets specification that the Ws2_32.dll can support
	char szDescription[WSADESCRIPTION_LEN+1];
	char szSystemStatus[WSASYSSTATUS_LEN+1];
	unsigned short iMaxSockets;
	unsigned short iMaxUdpDg;
	char FAR * lpVendorInfo;
	}WSADATA, *LPWSADATA;
	*/
	WSADATA wsaData;
	//#define MAKEWORD(a,b) ((WORD) (((BYTE) (a)) | ((WORD) ((BYTE) (b))) << 8)) 
	WORD version = MAKEWORD(2, 0);
	int ret = WSAStartup(version, &wsaData);//win sock start up
	if (ret) {
		//OutputDebugStringA("Initilize winsock error !\n");
		return -1;
	}
#endif
	return 0;
}
//this is just for windows
int ODSocket::Clean()
{
#ifdef WIN32
	return (WSACleanup());
#endif
	return 0;
}
ODSocket& ODSocket::operator = (SOCKET s)
{
	m_sock = s;
	return (*this);
}
ODSocket::operator SOCKET ()
{
	return m_sock;
}
//create a socket object win/lin is the same
// af:
bool ODSocket::Create(int af, int type, int protocol)
{
	m_sock = socket(af, type, protocol);
#ifdef WIN32
	u_long noBlock = 1;//设置非阻塞
	ioctlsocket(m_sock, FIONBIO, &noBlock);
#else
	int flags = fcntl(m_sock, F_GETFL, 0);
	fcntl(m_sock, F_SETFL, flags | O_NONBLOCK);
#endif
	if (m_sock == INVALID_SOCKET) {
		return false;
	}
	return true;
}
bool ODSocket::Connect(const char* ip, unsigned short port)
{
	struct sockaddr_in svraddr;
	svraddr.sin_family = AF_INET;
	svraddr.sin_addr.s_addr = inet_addr(ip);
	svraddr.sin_port = htons(port);
	fd_set wtfds, recvfds;
	timeval _timout = { 3,0 };
	FD_ZERO(&wtfds);
	FD_ZERO(&recvfds);
	/*_timout.tv_sec = 3;
	_timout.tv_usec = 0;*/
	FD_SET(m_sock, &wtfds);
	FD_SET(m_sock, &recvfds);
#ifdef WIN32
	int ret = connect(m_sock, (struct sockaddr*)&svraddr, sizeof(svraddr));//return ?
	if (!ret)
	{
		//OutputDebugStringA("connect immediately\n");
	}
	else if (ret<0 && WSAGetLastError() == WSAEWOULDBLOCK)
	{
		int iRet1 = select(0, NULL, &wtfds, NULL, &_timout);
		if (iRet1 < 0)
		{
			//	OutputDebugStringA("connect error\n");
			return false;
		}
		else if (!iRet1)
		{
			//	OutputDebugStringA("timeout error\n");
			return false;
		}
		else
		{
			if (FD_ISSET(m_sock, &wtfds))
			{
				//		OutputDebugStringA("connect success\n");
			}
		}
	}
#else
	int ret = connect(m_sock, (struct sockaddr*)&svraddr, sizeof(svraddr));//return ?
	if (!ret)
	{
		//OutputDebugStringA("connect immediately\n");
	}
	else if (ret < 0 && errno == EINPROGRESS)
	{
		int iRet1 = select(m_sock + 1, &recvfds, &wtfds, NULL, &_timout);
		if (iRet1 < 0)
		{			//OutputDebugStringA("connect error\n")
			return false;
		}
		else if (!iRet1)
		{
			//OutputDebugStringA("timeout error\n");
			return false;
		}
		else
		{
			if (FD_ISSET(m_sock, &recvfds))
				return false;
			if (FD_ISSET(m_sock, &wtfds))
			{
				//	OutputDebugStringA("connect success\n");
			}
		}
	}
#endif
	/*if (ret == SOCKET_ERROR) {
	return false;
	}*/
	return true;
}
bool ODSocket::Bind(unsigned short port)
{
	struct sockaddr_in svraddr;
    svraddr.sin_family = AF_INET;
	svraddr.sin_addr.s_addr = INADDR_ANY;
	svraddr.sin_port = htons(port);
	socklen_t opt = 1;
	if (setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) < 0)
		return false;
	int ret = bind(m_sock, (struct sockaddr*)&svraddr, sizeof(svraddr));
	if (ret == SOCKET_ERROR) {
	return false;
	}
	return true;
}
//for server
bool ODSocket::Listen(int backlog)
{
	int ret = listen(m_sock, backlog);
	if (ret == SOCKET_ERROR) {
		return false;
	}
	return true;
}
bool ODSocket::Accept(ODSocket& s, char* fromip)
{
	struct sockaddr_in cliaddr;
	socklen_t addrlen = sizeof(cliaddr);
	SOCKET sock = accept(m_sock, (struct sockaddr*)&cliaddr, &addrlen);
	if (sock == SOCKET_ERROR)
	{
		return false;
	}
#ifdef WIN32
	u_long noBlock = 1;
	ioctlsocket(sock, FIONBIO, &noBlock);
#else
	int flags = fcntl(sock, F_GETFL, 0);
	fcntl(sock, F_SETFL, flags | O_NONBLOCK);
#endif
	s = sock;
	if (fromip != NULL)
		sprintf(fromip, "%s", inet_ntoa(cliaddr.sin_addr));
	return true;
}
int ODSocket::Send(const char* buf, int len, int flags)
{
	return send(m_sock, buf, len, flags);
	/*int bytes;
	int count = 0;
    while (count < len) {
	bytes = send(m_sock, buf + count, len - count, flags);
	if (bytes == -1)
	return -1;
	else if (bytes == 0)
	return 0;
	count += bytes;
	}
	return count;*/
}
int ODSocket::Recv(char* buf, int len, int flags)
{
	return (recv(m_sock, buf, len, flags));
}
int ODSocket::SendTo(char * buf, int len, int flags, char* to, int port)
{
	sockaddr_in svraddr;
	socklen_t length = sizeof(svraddr);
	svraddr.sin_family = AF_INET;
	svraddr.sin_addr.s_addr = (to == NULL) ? INADDR_BROADCAST : inet_addr(to);//指定IP或广播地址
	svraddr.sin_port = htons(port);
	int bytes;
	bytes = sendto(m_sock, buf, len, 0, (sockaddr*)&svraddr, length);
	return bytes;
}
int ODSocket::RecvFrom(char * buf, int len, int flags, std::string &from, int &port)
{
	sockaddr_in svraddr;
	socklen_t length = sizeof(svraddr);
	int bytes;
	bytes = recvfrom(m_sock, buf, len, flags, (sockaddr*)&svraddr, &length);
	from = inet_ntoa(svraddr.sin_addr);
	port = ntohl(svraddr.sin_port);
	return bytes;
}
int ODSocket::Close()
{
#ifdef WIN32
	return (closesocket(m_sock));
#else
	return (close(m_sock));
#endif
}
int ODSocket::GetError()
{
#ifdef WIN32
	return (WSAGetLastError());
#else
	return (0);
#endif
}
bool ODSocket::DnsParse(const char* domain, char* ip)
{
	struct hostent* p;
	if ((p = gethostbyname(domain)) == NULL)
		return false;
	sprintf(ip,
		"%u.%u.%u.%u",
		(unsigned char)p->h_addr_list[0][0],
		(unsigned char)p->h_addr_list[0][1],
		(unsigned char)p->h_addr_list[0][2],
		(unsigned char)p->h_addr_list[0][3]);
	return true;
}