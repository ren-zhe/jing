#include "sock.h"  
#include"cocos2d.h"
//构造函数  
Sock::Sock() :_fd(INVALID_SOCKET)
{

#ifdef WIN32  

	//初始化Windoes下的Sock  
	static bool winInit = false;
	if (!winInit)
	{
		winInit = true;

		WSADATA data;
		WSAStartup(MAKEWORD(2, 2), &data);
	}
#endif  
}


//虚构函数  
Sock::~Sock()
{
	if (isValidSocket())
	{
		cocos2d::log("closesocket");
		close();
	}
}