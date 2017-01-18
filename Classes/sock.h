#ifndef __Sock_H__  
#define __Sock_H__  

#ifdef WIN32  
#include <WinSock2.h>  
#define SOCK_TYPE SOCKET  
#else  
#define SOCK_TYPE int  
#define INVALID_SOCKET -1  
#endif  

#define MyLog(...)   

//创建Sock类  
class Sock
{
public:
	//构造函数  
	Sock();
	//析构函数  
	virtual ~Sock();
	//设置成非阻塞模式  
	bool setNonBlock()
	{
		SOCK_TYPE fd = _fd;
#ifdef WIN32  
		BOOL flg = TRUE;
		//控制Socket的模式  
		if (ioctlsocket(fd, FIONBIO, (unsigned long *)&flg) != 0)
		{
			return false;
		}
		return true;
#else  
		int flags = fcntl(fd, F_GETFL, 0);
		flags |= O_NONBLOCK;
		return fcntl(fd, F_SETFL, flags) != -1;
#endif  
	}
	//关闭Sock  
	void close()
	{
#ifdef WIN32  
		closesocket(_fd);
#else  
		::close(_fd);
#endif  
	}

	void attach(SOCK_TYPE fd)
	{
		_fd = fd;
	}

	//判断Sock是否合法  
	bool isValidSocket()
	{
		return _fd != INVALID_SOCKET;
	}

protected:

	SOCK_TYPE _fd;
};


//客户端的Sock  
class SockClient : public Sock
{
public:
	//构造函数  
	SockClient(unsigned short port = 0, const char* ip = NULL)
	{
		if (port == 0 && ip == NULL)
		{
			return;
		}

		//连接  
		connect(port, ip);
	}

	//析构函数  
	~SockClient() {}


	//连接  
	//第一个参数：端口  
	//第二个参数：ip地址  
	int connect(unsigned short port, const char* ip)
	{
		//分配一个Socket  
		//第一个参数：AF_INET表示指定地址族(地址描述)  
		//第二个参数：SOCK_STREAM表示流式套接字TCP(Socket类型)  
		//第三个参数：0(协议)  
		_fd = socket(AF_INET, SOCK_STREAM, 0);

		//地址信息结构  
		struct sockaddr_in addr;

		//地址家族  
		addr.sin_family = AF_INET;

		//端口号  
		addr.sin_port = htons(port);

		//主机地址  
		addr.sin_addr.s_addr = inet_addr(ip);

		//连接  
		int ret = ::connect(_fd, (struct sockaddr*)&addr, sizeof(addr));

		if (ret < 0)
		{
			MyLog("connect error errno=%d", errno);
			return -1;
		}

		return 0;
	}

	//接收  
	int recv(char* buf, int len)
	{
		return ::recv(_fd, buf, len, 0);
	}

	//发送  
	int send(const char* buf, int len)
	{
		return ::send(_fd, buf, len, 0);
	}
};

//服务器端的Sock  
class SockServer :public Sock
{
public:
	//构造函数  
	SockServer(unsigned short port, const char* ip = NULL)
	{
		//监听  
		listen(port, ip);
	}

	//虚构函数  
	~SockServer() {}

	//接受连接  
	SockClient* accept()
	{
		//接受客户端的发送请求,等待客户端发送connect请求  
		SOCK_TYPE fd = ::accept(_fd, NULL, NULL);

		if (fd != INVALID_SOCKET)
		{
			//创建一个SockClient  
			SockClient* ret = new SockClient;
			ret->attach(fd);
			return ret;
		}
		return NULL;
	}

protected:
	//监听  
	//第一个参数：端口  
	//第二个参数：ip地址  
	int listen(unsigned short port, const char* ip = NULL)
	{
		//分配一个Socket  
		//第一个参数：AF_INET表示指定地址族(地址描述)  
		//第二个参数：SOCK_STREAM表示流式套接字TCP(Socket类型)  
		//第三个参数：0(协议)  
		_fd = socket(AF_INET, SOCK_STREAM, 0);

		//地址信息结果  
		struct sockaddr_in addr;

		//地址家族  
		addr.sin_family = AF_INET;

		//端口号  
		addr.sin_port = htons(port);

		if (ip == NULL)
		{
			//设置一个不确定的ip地址  
			addr.sin_addr.s_addr = INADDR_ANY;
		}
		else
		{
			//将ip地址转换为32位二进制网络字节序的IPV4地址  
			addr.sin_addr.s_addr = inet_addr(ip);
		}

		//绑定  
		int ret = bind(_fd, (struct sockaddr*)&addr, sizeof(addr));

		if (ret < 0)
		{
			MyLog("bind error");
			return -1;
		}

		//设置成非阻塞  
		//this->setNonBlock();

		//监听  
		::listen(_fd, 10);

		return 0;
	}
};

#endif  
