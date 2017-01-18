#include"LineScene.h"
#include"net.h"
#include"OnlineGameScene.h"
#include"HelloWorldScene.h"
USING_NS_CC;
Scene* LineScene::createScene()
{
	auto scene = Scene::create();
	auto layer = LineScene::create();
	scene->addChild(layer);
	return scene;
}
//void text()
//{
//	while (1)
//	{
//	
//		OutputDebugString(L"text \n");
//		Sleep(2000);
//	}
//}
bool LineScene::init()
{
	if (!Layer::init())
	{
		return false;
	}
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	Size visibleSize = Director::getInstance()->getVisibleSize();
	auto bg = Sprite::create("background.png");
	bg->setPosition(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2);
	addChild(bg);

	auto labelMakeServer = Label::createWithSystemFont("Make Server", "Arial", 60);
	auto menuMakeServer = MenuItemLabel::create(labelMakeServer, CC_CALLBACK_1(LineScene::makeServer, this));

	auto labelMakeClient = Label::createWithSystemFont("Make Client", "Arial", 60);
	auto menuMakeClient = MenuItemLabel::create(labelMakeClient, CC_CALLBACK_1(LineScene::makeClient, this));

	auto labelStart = Label::createWithSystemFont("Start", "Arial", 60);
	auto menuStart = MenuItemLabel::create(labelStart, CC_CALLBACK_1(LineScene::start, this));

	auto labelExit = Label::createWithSystemFont("Exit", "Arial", 60);
	auto menuExit = MenuItemLabel::create(labelExit, [](Ref* pSender) {
		
		Net::getInstance()->exit = true;
		/*Net::getInstance()->destroyInstance();*/
#ifdef WIN32
		Sleep(500);
#else
		usleep(500);
#endif
		Net::getInstance()->DeleteSocket();
		auto scene = HelloWorld::createScene();
		auto transitionScene = TransitionFade::create(1.0f, scene);
		Director::getInstance()->replaceScene(transitionScene);
	});
	auto menu = Menu::create(menuMakeServer, menuMakeClient,menuStart, menuExit, NULL);
	menu->alignItemsVertically();
	addChild(menu);
	Net::getInstance()->createUdpSocket();
	std::thread recvfromThread(&LineScene::recvBroadcast, this);
	recvfromThread.detach();
	schedule(schedule_selector(LineScene::manage), 1.0f);
	/*std::thread textThread(&text);
	textThread.detach();*/
	return true;
}

void LineScene::manage(float dt)//为什么换成NetLine不行
{
	
	/*if (!Net::getInstance()->connectSocket)
		return;*/
	if (Net::getInstance()->connect)
	{log("manage");
		if (getChildByTag(5) == NULL)
		{
			auto connectLabel = Label::createWithSystemFont("Connect OK", "Arial", 20);
			connectLabel->setPosition(Director::getInstance()->getVisibleSize().width, 0);
			connectLabel->setAnchorPoint(Vec2(1, 0));
			connectLabel->setTag(5);
			addChild(connectLabel);
		}
	}
	if (!Net::getInstance()->str.empty())
	{
		log("not empty");
		Net::getInstance()->m_mutex.lock();
		for (std::vector<std::string>::iterator it = Net::getInstance()->str.begin(); it != Net::getInstance()->str.end(); it++)
		{
			std::string temp = *it;
			if (temp.size() >= 3)
			{
				log("recv     99999999999999");
				if (temp[0] == '9' && temp[1] == '9' && temp[2] == '9')
				{
					log("server is on");
					auto labelConnect = Label::createWithSystemFont("Server is on", "Arial", 20);
					labelConnect->setPosition(0, 0);
					labelConnect->setAnchorPoint(Vec2::ZERO);
					addChild(labelConnect);
				}
				else if (temp[0] == '1' && temp[1] == '5' &&temp[2] == '5')
				{
					auto scene = OnlineGameScene::createScene();
					auto transitionScene = TransitionFadeTR::create(1.0f, scene);
					Director::getInstance()->replaceScene(transitionScene);
					break;
				}
			}
		}
		Net::getInstance()->str.clear();
		Net::getInstance()->m_mutex.unlock();
	}
}

void LineScene::start(Ref * pSender)
{
	if (Net::getInstance()->connect)
	{
		if (!Net::getInstance()->clientList.empty())
		{
			Connection* client_1 = *Net::getInstance()->clientList.begin();
			Net::getInstance()->m_mutex1.lock();
			client_1->sendBuffer[client_1->sendBytes] = '1';
			client_1->sendBuffer[client_1->sendBytes + 1] = '5';
			client_1->sendBuffer[client_1->sendBytes + 2] = '5';
			client_1->sendBuffer[client_1->sendBytes + 3] = '\0';
			client_1->sendBytes += 4;
			Net::getInstance()->m_mutex1.unlock();
		}
		auto scene = OnlineGameScene::createScene();
		auto transitionScene = TransitionFadeTR::create(1.0f, scene);
		Director::getInstance()->replaceScene(transitionScene);
	}
	
}

void LineScene::makeServer(Ref* pSender)
{
	if (!Net::getInstance()->makeServer())
	{
		return;
	}
		/*auto labelConnect = Label::createWithSystemFont("Server is on", "Arial", 20);
		labelConnect->setPosition(0, 0);
		labelConnect->setAnchorPoint(Vec2::ZERO);
		addChild(labelConnect);
	}
	else
	{
		log("make server error");
	}*/
}

void LineScene::makeClient(Ref* pSender)
{
	if (!Net::getInstance()->makeClient())
		return;
}

void LineScene::recvBroadcast()
{
	Net::getInstance()->RecvBroadcast();
}
