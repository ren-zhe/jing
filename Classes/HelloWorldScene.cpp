#include "HelloWorldScene.h"
#include"GameScene.h"
#include"LineScene.h"
USING_NS_CC;

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance


bool HelloWorld::init()
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
	auto labelGameName = Label::createWithSystemFont("Tic Tac Toe", "Arial", 70);
	labelGameName->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height - 100));
	addChild(labelGameName);
	auto singleGameLabel = Label::createWithSystemFont("Local Game", "Arial", 50);
	auto menuSingleItem = MenuItemLabel::create(singleGameLabel, CC_CALLBACK_1(HelloWorld::LocalGame, this));
	auto doubleGameLabel = Label::createWithSystemFont("Online Game", "Arial", 50);
	auto menuDoubleItem = MenuItemLabel::create(doubleGameLabel, CC_CALLBACK_1(HelloWorld::OnlineGame, this));
	auto labelExit = Label::createWithSystemFont("Exit", "Arial", 50);
	auto menuExit = MenuItemLabel::create(labelExit, [](Ref* pSender) {
		Director::getInstance()->end();
	});
	auto menu = Menu::create(menuSingleItem, menuDoubleItem,menuExit, NULL);
	menu->alignItemsVertically();
	addChild(menu);
	return true;
}

void HelloWorld::LocalGame(Ref* pSender)
{
	auto sc = GameScene::createScene();
	auto scene = TransitionFade::create(1.0f, sc);
	Director::getInstance()->replaceScene(scene);
}
void HelloWorld::OnlineGame(Ref* pSender)
{
	//auto bg = Sprite::create("onlineItem.png");
	//bg->setPosition(Director::getInstance()->getVisibleSize().width / 2, Director::getInstance()->getVisibleSize().height / 2);
	//addChild(bg);
	auto sc = LineScene::createScene();
	auto scene = TransitionFade::create(1.0f, sc);
	Director::getInstance()->replaceScene(scene);
	/*auto labelMakeServer = Label::create("Make Server", "Arial", 40);
	auto menuMakeServer = MenuItemLabel::create(labelMakeServer, CC_CALLBACK_1(HelloWorld::makeServer, this));
	auto labelMakeClient = Label::create("makeClient", "Arial", 40);
	auto menuMakeClient = MenuItemLabel::create(labelMakeClient, CC_CALLBACK_1(HelloWorld::makeClient, this));
	
	auto menu = Menu::create(menuMakeServer, menuMakeClient, NULL);
	menu->alignItemsVertically();
	addChild(menu);*/
}

////启动服务器  
//void HelloWorld::makeServer(Ref* pSender)
//{
//	this->_server = new SockServer(9888);
//
//	if (!this->_server->isValidSocket())
//	{
//		log("server ERR");
//	}
//	else
//	{
//		log("server OK");
//	}
//}
//
//
////启动客服端  
//void HelloWorld::makeClient(Ref* spSender)
//{
//	this->_client = new SockClient(9888, "127.0.0.1");
//
//	if (!this->_client->isValidSocket())
//	{
//		log("Client ERR");
//	}
//	else
//	{
//		log("Client OK");
//	}
//}
//
//
////接受连接  
//void HelloWorld ::Accept(Ref* pSender)
//{
//	this->_client = this->_server->accept();
//
//	if (!this->_client->isValidSocket())
//	{
//		log("Accept ERR");
//	}
//	else
//	{
//		log("Accept OK");
//	}
//}
//
//
////发送  
//void HelloWorld::Send(Ref* pSender)
//{
//	//发送信息  
//	int ret = this->_client->send("Hello", 6);
//	log("send: ret is %d", ret);
//}
//
//
////接收  
//void HelloWorld::Recv(Ref* pSender)
//{
//	char buf[128];
//
//	//接收信息  
//	int ret = this->_client->recv(buf, sizeof(buf));
//	log("recv buf is %s", buf);
//}
