#ifndef __ONLINEGAME_SCENE_H__
#define __ONLINEGAME_SCENE_H__

#include "cocos2d.h"
#include"GameScene.h"
//#include "ODSocket.h"
#include"net.h"
//#define PORTA 9100
//enum type
//{
//	tag_o = 0,
//	tag_x,
//	tag_none
//};
//enum gameState
//{
//	o_win = 0,
//	x_win,
//	none_win,
//	undone
//};
class OnlineGameScene : public cocos2d::Layer
{
public:
	OnlineGameScene();
	static Scene* createScene();
	virtual bool init();
	CREATE_FUNC(OnlineGameScene);
	virtual void onTouchEnded(Touch*, Event*);
	virtual bool onTouchBegan(Touch*, Event*);
	gameState getState();
	void restart();
	virtual void onExit();

	//启动服务器  
	//void makeServer(Ref*);

	//启动客服端  
	//void makeClient(Ref*);

	void First(Ref * pSender);
	/*void scheduleRecv(std::string temp);*/
	/*void Send(char*);*/
	void Manage(float dt);
	/*void accept();*/
	/*void SendBroadcast();
	void RecvBroadcast();
	void ResetFDSet(fd_set& fdRead, fd_set& fdWrite, fd_set& fdExcept, SOCKET sdListen, const ConnectionList& conns);
	int CheckAccept(const fd_set& fdRead, const fd_set& fdExcept, SOCKET sdListen, ConnectionList& conns);
	void CheckConn(const fd_set& fdRead, const fd_set& fdWrite, const fd_set& fdExcept, ConnectionList& conns);
	bool TryRead(Connection* pConn);
	bool TryWrite(Connection* pConn);
	bool PassiveShutdown(SOCKET sd, const char* buff, int len);*/
private:
	std::vector<std::vector<type> > data;
	type flag;
	bool myTurn;
	/*std::vector<std::string> str;*/
	/*std::mutex m_mutex;
	std::mutex m_mutex1;*/
	//static bool clean;
   /* ODSocket* listenSocket;
	ODSocket* connectSocket;
	ODSocket* udp;
	std::string serverIP;
	int serverPort;
	bool connect;
	ConnectionList conns;*/
};
#endif
