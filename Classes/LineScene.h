#ifndef __LINE_SCENE__
#define __LINE_SCENE__
#include"cocos2d.h"
//class NetLine : public Net
//{
//public:
//	virtual void manage(float dt);
//};
class LineScene :public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();
	virtual bool init();
	CREATE_FUNC(LineScene);
	void manage(float dt);
	void start(Ref* pSender);
	void makeServer(Ref* pSender);
	void makeClient(Ref* pSender);
	void recvBroadcast();
};
#endif
