#include "OnlineGameScene.h"
#include"HelloWorldScene.h"
#include"LineScene.h"
//bool OnlineGameScene::clean = false;
OnlineGameScene::OnlineGameScene()
{
	log("init");
	std::vector<type> temp(3);//初始化棋盘
	for (std::vector<type>::iterator it = temp.begin(); it != temp.end(); it++)
		*it = tag_none;
	for (int i = 0; i < 3; i++)
		data.push_back(temp);
	myTurn = false; //记录回合
	flag = tag_none;//保存选择的棋子
	Net::getInstance()->str.clear();//保存接受信息
	//connect = false;//记录连接状态
	//connectSocket = NULL;//连接套接字
	//listenSocket = NULL;//监听套接字
	//udp = NULL;//udp连接套接字
}
Scene* OnlineGameScene::createScene()
{
	Scene* scene = Scene::create();
	Layer* layer = OnlineGameScene::create();
	scene->addChild(layer);
	return scene;
}
bool OnlineGameScene::init()
{
	log("init function");
	std::cerr << "ok" << std::endl;
	if (!Layer::init())
	{
		return false;
	}
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	Size visibleSize = Director::getInstance()->getVisibleSize();
	auto bg = Sprite::create("backgroundGame.png");
	bg->setPosition(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2);
	addChild(bg);
	auto labelExit = Label::createWithSystemFont("Exit", "Arial", 20);
	auto menuExit = MenuItemLabel::create(labelExit, [](Ref* pSender) {
		
		Net::getInstance()->exit = true;
	/*	Net::getInstance()->destroyInstance();*/
#ifdef WIN32
		Sleep(500);
#else
		usleep(500);
#endif
		Net::getInstance()->DeleteSocket();
		auto scene = HelloWorld::createScene();
		auto sc = TransitionFade::create(1.0f, scene);
		Director::getInstance()->replaceScene(sc);
	});
	/*auto labelMakeServer = Label::createWithSystemFont("Make Server", "Arial", 20);
	auto menuMakeServer = MenuItemLabel::create(labelMakeServer, CC_CALLBACK_1(OnlineGameScene::makeServer, this));
	auto labelMakeClient = Label::createWithSystemFont("Make Client", "Arial", 20);
	auto menuMakeClient = MenuItemLabel::create(labelMakeClient, CC_CALLBACK_1(OnlineGameScene::makeClient, this));*/
	auto labelFirst = Label::createWithSystemFont("First", "Arial", 20);
	auto menuFirst = MenuItemLabel::create(labelFirst, CC_CALLBACK_1(OnlineGameScene::First, this));
	auto menu = Menu::create( menuFirst, menuExit, NULL);
	menu->alignItemsHorizontally();
	menu->setPosition(visibleSize.width / 2, visibleSize.height * 7 / 8);
	addChild(menu);
	auto flag = Sprite::create("o.png");
	flag->setTag(tag_o);
	flag->setScale(0.3);
	flag->setPosition(visibleSize.width / 2, visibleSize.height / 12);
	addChild(flag);
	auto label = Label::createWithSystemFont("turns", "Arial", 20);
	label->setPosition(visibleSize.width * 2 / 3, visibleSize.height / 12);
	addChild(label);
	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(OnlineGameScene::onTouchBegan, this);
	listener->onTouchEnded = CC_CALLBACK_2(OnlineGameScene::onTouchEnded, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
	/*if (!Net::getInstance()->createUdpSocket())
		return false;*/
	/*std::thread recvfromThread(&OnlineGameScene::RecvBroadcast, this);
	recvfromThread.detach();*/
	//std::thread recvThread(&OnlineGameScene::scheduleRecv, this);//std::mem_fn
	//recvThread.detach();
	schedule(schedule_selector(OnlineGameScene::Manage), 0.2);
	return true;
}
gameState OnlineGameScene::getState()
{
	gameState winner = undone;
	for (int i = 0; i < 3; i++)//horizontal
	{
		type temp = data.at(i).at(0);
		int j = 1;
		for (; j < 3; j++)
		{
			if (data.at(i).at(j) != temp)
				break;
		}
		if (j == 3 && temp != tag_none)
		{
			winner = temp == tag_x ? x_win : o_win;
			return winner;
		}
	}
	for (int i = 0; i < 3; i++)//vertical
	{
		type temp = data.at(0).at(i);
		int j = 1;
		for (; j < 3; j++)
		{
			if (data.at(j).at(i) != temp)
				break;
		}
		if (j == 3 && temp != tag_none)
		{
			winner = temp == tag_x ? x_win : o_win;
			return winner;
		}
	}
	if (data.at(0).at(0) == data.at(1).at(1) && data.at(0).at(0) == data.at(2).at(2) && data.at(0).at(0) != tag_none)
	{
		winner = data.at(0).at(0) == tag_x ? x_win : o_win;
		return winner;
	}
	if (data.at(0).at(2) == data.at(1).at(1) && data.at(0).at(2) == data.at(2).at(0) && data.at(2).at(0) != tag_none)
	{
		winner = data.at(0).at(2) == tag_x ? x_win : o_win;
		return winner;
	}
	bool doneTag = true;
	for (int i = 0; i < 3; i++)
	{
		if (!doneTag)
			break;
		for (int j = 0; j < 3; j++)
		{
			if (data[i][j] == tag_none)
			{
				doneTag = false;
				break;
			}
		}
	}
	if (doneTag && winner == undone)
		winner = none_win;
	return winner;
}
void OnlineGameScene::restart()
{
	auto labelRestart = Label::createWithSystemFont("Restart", "Arial", 20);
	auto menuItemRestart = MenuItemLabel::create(labelRestart, [=](Ref* pSender) {
		//clean = true;
		if (!Net::getInstance()->clientList.empty())
		{
			Connection* client_1 = *Net::getInstance()->clientList.begin();
			Net::getInstance()->m_mutex1.lock();
			client_1->sendBuffer[client_1->sendBytes] = '1';
			client_1->sendBuffer[client_1->sendBytes + 1] = '4';
			client_1->sendBuffer[client_1->sendBytes + 2] = '4';
			client_1->sendBuffer[client_1->sendBytes + 3] = '\0';
			client_1->sendBytes += 4;
			Net::getInstance()->m_mutex1.unlock();
		}
#ifdef WIN32
		Sleep(500);
#else
		usleep(500);
#endif
		auto scene = OnlineGameScene::createScene();
		auto sc = TransitionFade::create(1.0f, scene);
		Director::getInstance()->replaceScene(sc);
		/*flag = tag_none;
		myTurn = false;
		data.clear();*/
	});
	auto labelExit = Label::createWithSystemFont("Exit", "Arial", 20);
	auto menuItemExit = MenuItemLabel::create(labelExit, [=](Ref* pSender) {
			
		Net::getInstance()->exit = true;
		/*Net::getInstance()->destroyInstance();*/
#ifdef WIN32
		Sleep(500);
#else
		usleep(500);
#endif
		Net::getInstance()->DeleteSocket();	
		auto scene = HelloWorld::createScene();
		auto sc = TransitionFade::create(1.0f, scene);
		Director::getInstance()->replaceScene(sc);
	});
	auto menu = Menu::create(menuItemRestart, menuItemExit, NULL);
	menu->alignItemsHorizontallyWithPadding(100);
	menu->setPosition(Director::getInstance()->getVisibleSize().width / 2, Director::getInstance()->getVisibleSize().height / 2);
	addChild(menu);

}
void OnlineGameScene::onTouchEnded(Touch* touch, Event* event)
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 t = touch->getLocation();
	int i = (t.x - visibleSize.width / 24) / (11 * visibleSize.width / 36);
	int j = (visibleSize.height - t.y - 13 * visibleSize.width / 36) / (11 * visibleSize.width / 36);
	if (i >= 0 && i < 3 && j >= 0 && j < 3 && data[i][j] == tag_none)
	{
		Connection* client_1 = *Net::getInstance()->clientList.begin();
		Net::getInstance()->m_mutex1.lock();
		client_1->sendBuffer[client_1->sendBytes] = '1';
		client_1->sendBuffer[client_1->sendBytes + 1] = '0' + i;
		client_1->sendBuffer[client_1->sendBytes + 2] = '0' + j;
		client_1->sendBuffer[client_1->sendBytes + 3] = '\0';
		client_1->sendBytes += 4;
		Net::getInstance()->m_mutex1.unlock();
		/*char temp[4];
		temp[0] = '1';
		temp[1] = '0' + i;
		temp[2] = '0' + j;
		temp[3] = '\0';*/
		log("sendok");
		//log(client_1->sendBuffer);
		std::string tmp;
		if (flag == tag_o)
		{
			tmp = "o.png";
		}
		else
		{
			tmp = "x.png";
		}
		auto flagSprite = Sprite::create(tmp);
		flagSprite->setAnchorPoint(Vec2(0, 1));
		flagSprite->setPosition(i * (11 * visibleSize.width / 36) + visibleSize.width / 24,
			visibleSize.height - (11 * visibleSize.width / 36 * j + 13 * visibleSize.width / 36));
		addChild(flagSprite);
		//std::thread t(&OnlineGameScene::Send, this, temp);
		//t.detach();
		data.at(i).at(j) = flag;
		this->removeChildByTag(flag);//移除上一次的标记
		auto nextFlag = flag == tag_o ? tag_x : tag_o;

		myTurn = false;
		Size visibleSize = Director::getInstance()->getVisibleSize();
		if (nextFlag == tag_o)
		{
			tmp = "o.png";
		}
		else
		{
			tmp = "x.png";
		}
		auto* sprite = Sprite::create(tmp);
		sprite->setTag(nextFlag);
		sprite->setScale(0.3);
		sprite->setPosition(visibleSize.width / 2, visibleSize.height / 12);
		addChild(sprite);
		gameState state = getState();
		if (state != undone)
		{
			switch (state)
			{
			case o_win:
				tmp = "o_win.png";
				break;
			case x_win:
				tmp = "x_win.png";
				break;
			case none_win:
				tmp = "none_win.png";
				break;
			}
			auto sprite = Sprite::create(tmp.c_str());
			sprite->setPosition(visibleSize.width / 2, visibleSize.height / 2);
			addChild(sprite);
			restart();
		}
	}
}
bool OnlineGameScene::onTouchBegan(Touch*, Event*)
{
	if (!myTurn)
		return false;
	gameState  state = getState();
	if (state == undone)
		return true;
	else
		return false;
}
//启动服务器  
//void OnlineGameScene::makeServer(Ref* pSender)
//{
//	if (listenSocket)
//		return;
//	listenSocket = new ODSocket();
//	if (!listenSocket)
//	{
//		log("new error");
//		return;
//	}
//	if (listenSocket->Init() == -1)
//	{
//		log("Init error");
//		return;
//	}
//	if (!listenSocket->Create(AF_INET, SOCK_STREAM, 0))
//	{
//		log("Create listensocket error");
//		return;
//	}
//	int port = DEFAULTPORT;
//	if (!listenSocket->Bind(port))
//	{
//		log("Bind error");
//		return;
//	}
//	if (!listenSocket->Listen())
//	{
//		log("Listen error");
//		return;
//	}
//	std::thread acceptThread(&OnlineGameScene::accept, this);
//	acceptThread.detach();
//	std::thread broadcastThread(&OnlineGameScene::SendBroadcast, this);
//	broadcastThread.detach();
//}

void OnlineGameScene::onExit()
{
	/*clean = true;*/
	/*if (listenSocket != NULL)
	{
	delete listenSocket;
	listenSocket = NULL;
	}
	if (connectSocket != NULL)
	{
	delete connectSocket;
	connectSocket = NULL;
	}
	if (udp != NULL)
	{
	delete udp;
	udp = NULL;
	}
	clean = false;*/
}
//启动客服端  
//void OnlineGameScene::makeClient(Ref* spSender)
//{
//	if (serverIP.empty())
//		return;
//	if (connectSocket != NULL)
//		return;
//	connectSocket = new ODSocket();
//	if (connectSocket == NULL)
//	{
//		log("new error");
//		return;
//	}
//	if (connectSocket->Init() == -1)
//	{
//		log("Init error");
//		return;
//	}
//	if (!connectSocket->Create(AF_INET, SOCK_STREAM, 0))
//	{
//		log("Create error");
//		return;
//	}
//	std::string temp = serverIP;
//	/*if (!connectSocket->Connect(temp.c_str(), DEFAULTPORT))
//	{
//	log("Connect error1");
//	return;
//	}*/
//	
//	//thread
//	std::thread rr(&OnlineGameScene::scheduleRecv,this,temp);
//	rr.detach();
//}
void OnlineGameScene::First(Ref * pSender)
{
	/*if (connectSocket != NULL)
	{*/
	if (!Net::getInstance()->clientList.empty() && flag == tag_none)
	{
		log("first");
		Connection* client_1 = *Net::getInstance()->clientList.begin();
	
		client_1->sendBuffer[client_1->sendBytes] = '1';
		client_1->sendBuffer[client_1->sendBytes + 1] = '3';
		client_1->sendBuffer[client_1->sendBytes + 2] = '3';
		client_1->sendBuffer[client_1->sendBytes + 3] = '\0';
		client_1->sendBytes += 4;
		//log(client_1->sendBuffer);
		//connectSocket->Send("133", 4);
		myTurn = true;
		flag = tag_o;
		char c = flag == tag_x ? 'x' : 'o';
		std::string temp = StringUtils::format("%c.png", c);
		auto sprite = Sprite::create(temp);
		sprite->setScale(0.3);
		sprite->setPosition(Director::getInstance()->getVisibleSize().width / 3, Director::getInstance()->getVisibleSize().height / 5);
		addChild(sprite);
		auto label = Label::createWithSystemFont("I choose ", "Arial", 20);
		label->setPosition(Director::getInstance()->getVisibleSize().width / 5, Director::getInstance()->getVisibleSize().height / 5);
		addChild(label);
	}
}

//void OnlineGameScene::scheduleRecv(std::string temp)
//{
//	/*while (!connect)
//	{
//		if (connectSocket->Connect(temp.c_str(), DEFAULTPORT))
//		{
//			connect = true;
//			log("connect ok");
//		}
//	}
//	fd_set fdRead, fdWrite, fdExcept;
//	SOCKET sd = *connectSocket;
//	conns.push_back(new Connection(sd));
//	while (true)
//	{
//		if (clean)
//		{
//			closesocket(sd);
//
//		}
//		log("circle");
//		FD_ZERO(&fdRead);
//		FD_ZERO(&fdWrite);
//		FD_ZERO(&fdExcept);
//
//		ConnectionList::const_iterator it = conns.begin();
//		if ((*it)->recvBytes < BUFFERSIZE)
//		{
//			FD_SET((*it)->hSocket, &fdRead);
//		}
//		if ((*it)->sendBytes > 0)
//		{
//			FD_SET((*it)->hSocket, &fdWrite);
//		}
//		FD_SET((*it)->hSocket, &fdExcept);
//		timeval time{ 1, 0 };
//		int ret = select(0, &fdRead, &fdWrite, &fdExcept, &time);
//		log("after select");
//		if (ret < 0)
//		{
//			log("select error");
//			std::cout << "select error" << WSAGetLastError() << std::endl;
//			break;
//		}
//		else if (ret == 0)
//			continue;
//		CheckConn(fdRead, fdWrite, fdExcept, conns);
//	}*/
//	//while (1)
//	//{
//	//	if (clean)
//	//		break;
//	//	if (!connect)
//	//		continue;
//	//	char temp[BUFFERSIZE];
//	//	int recvNum = connectSocket->Recv(temp, sizeof(temp));
//	//	if (recvNum == 0)
//	//	{
//	//		if (listenSocket != NULL)
//	//		{
//	//			delete listenSocket;
//	//			listenSocket = NULL;
//	//		}
//	//		if (connectSocket != NULL)
//	//		{
//	//			delete connectSocket;
//	//			connectSocket = NULL;
//	//		}
//	//		if (udp != NULL)
//	//		{
//	//			delete udp;
//	//			udp = NULL;
//	//		}
//	//		//断开连接的处理
//	//		break;
//	//	}
//	//	else if (recvNum >= 3)
//	//	{
//	//		if (temp[0] == '1')//合法消息
//	//		{
//	//			if (temp[1] == '3' && temp[2] == '3')//first消息
//	//			{
//	//				myTurn = false;
//	//				flag = tag_x;
//	//				log("recv first");
//	//				m_mutex.lock();
//	//				str.push_back(temp);
//	//				m_mutex.unlock();
//	//			}
//	//			else if (temp[2] >= '0' && temp[2] < '3' && temp[1] >= '0' && temp[1] < '3')//对方下棋子消息
//	//			{
//	//				myTurn = true;
//	//				m_mutex.lock();
//	//				str.push_back(temp);
//	//				m_mutex.unlock();
//	//				auto nextFlag = flag == tag_o ? tag_x : tag_o;//获取对方棋子类型
//	//				data[temp[1] - '0'][temp[2] - '0'] = nextFlag;
//	//				log("recv keys");
//	//			}
//	//		}
//	//	}
//	//	memset(temp, '0', sizeof(temp));
//	//}
//}

//void OnlineGameScene::Send(char*temp)
//{
//	while (1)
//	{
//		int err;
//		err = connectSocket->Send(temp, strlen(temp) + 1);
//#ifdef WIN32
//		if (err == WSAEWOULDBLOCK)
//			continue;
//		else if (err < 0)
//		{
//			log("error");
//			break;
//		}
//		else
//			break;
//#else
//		if (err == EINPROGRESS)
//			continue;
//		else if (err < 0)
//		{
//			log("error");
//			break;
//		}
//		else
//			break;
//#endif
//	}
//}

void OnlineGameScene::Manage(float dt)
{
	/*if (connect)
	{
		if (getChildByTag(5) == NULL)
		{
			auto connectLabel = Label::createWithSystemFont("Connect OK", "Arial", 20);
			connectLabel->setPosition(Director::getInstance()->getVisibleSize().width, 0);
			connectLabel->setAnchorPoint(Vec2(1, 0));
			connectLabel->setTag(5);
			addChild(connectLabel);
		}
	}*/
	if (!Net::getInstance()->str.empty())
	{
		Net::getInstance()->m_mutex.lock();
		for (std::vector<std::string>::iterator it = Net::getInstance()->str.begin(); it != Net::getInstance()->str.end(); it++)
		{
			std::string temp = *it;
			if (temp.size() >= 3)
			{
				/*if (temp[0] == '9' && temp[1] == '9' && temp[2] == '9')
				{
					auto labelConnect = Label::createWithSystemFont("Server is on", "Arial", 20);
					labelConnect->setPosition(0, 0);
					labelConnect->setAnchorPoint(Vec2::ZERO);
					addChild(labelConnect);
				}*/
				if (temp[1] == '3' && temp[2] == '3')
				{
					myTurn = false;
					flag = tag_x;
					log("manage first");
					char c = flag == tag_x ? 'x' : 'o';
					std::string temp = StringUtils::format("%c.png", c);
					auto sprite = Sprite::create(temp);
					sprite->setScale(0.3);
					sprite->setPosition(Director::getInstance()->getVisibleSize().width / 3, Director::getInstance()->getVisibleSize().height / 5);
					addChild(sprite);
					auto label = Label::createWithSystemFont("I choose ", "Arial", 20);
					label->setPosition(Director::getInstance()->getVisibleSize().width / 5, Director::getInstance()->getVisibleSize().height / 5);
					addChild(label);

				}
				else if (temp[1] == '4' && temp[2] == '4')
				{
					auto scene = OnlineGameScene::createScene();
					auto sc = TransitionFade::create(1.0f, scene);
					Director::getInstance()->replaceScene(sc);
					break;
				}
				else
				{
					log("manage qizi");
					myTurn = true;
					//auto nextFlag = flag == tag_o ? tag_x : tag_o;//获取对方棋子类型
					//data[temp[1] - '0'][temp[2] - '0'] = nextFlag;
					Size visibleSize = Director::getInstance()->getVisibleSize();
					int i = temp[1] - '0';
					int j = temp[2] - '0';
					data[i][j] = flag == tag_o ? tag_x : tag_o;//temp[0] == '2'
					char tag = flag == tag_o ? 'x' : 'o';
					std::string file = StringUtils::format("%c.png", tag);
					auto sprite = Sprite::create(file);
					sprite->setAnchorPoint(Vec2(0, 1));
					sprite->setPosition((temp[1] - '0') * 11 * visibleSize.width / 36 + visibleSize.width / 24,
						visibleSize.height - (11 * visibleSize.width / 36 * (temp[2] - '0') + 13 * visibleSize.width / 36));
					addChild(sprite);
					this->removeChildByTag(data[i][j]);
					std::string t;
					if (flag == tag_o)
					{
						t = "o.png";
					}
					else
					{
						t = "x.png";
					}
					auto f = Sprite::create(t);
					f->setTag(flag);
					f->setScale(0.3);
					f->setPosition(visibleSize.width / 2, visibleSize.height / 12);
					addChild(f);
					gameState state = getState();
					if (state != undone)
					{
						std::string tm;
						switch (state)
						{
						case o_win:
							tm = "o_win.png";
							break;
						case x_win:
							tm = "x_win.png";
							break;
						case none_win:
							tm = "none_win.png";
							break;
						}
						auto sprite = Sprite::create(tm);
						sprite->setPosition(visibleSize.width / 2, visibleSize.height / 2);
						addChild(sprite);
						restart();
					}
				}
			}
		}
		Net::getInstance()->str.clear();
		Net::getInstance()->m_mutex.unlock();
	}
}

//void OnlineGameScene::accept()
//{
//	fd_set fdRead, fdWrite, fdExcept;
//
//	SOCKET sdListen = *listenSocket;
//	while (true)
//	{
//		if (clean)
//		{
//			closesocket(sdListen);
//		}
//		log("circle");
//		ResetFDSet(fdRead, fdWrite, fdExcept, sdListen, conns);
//		timeval time = { 1,0 };
//		int ret = select(0, &fdRead, &fdWrite, &fdExcept, &time);
//		if (ret < 0)
//		{
//			log("select error");
//			std::cout << "select error" << WSAGetLastError() << std::endl;
//			break;
//		}
//		else if (ret == 0)
//			continue;
//		
//		ret = CheckAccept(fdRead, fdExcept, sdListen, conns);
//		if (ret == SOCKET_ERROR)
//		{
//			log("checkaccept error");
//			break;
//		}
//		CheckConn(fdRead, fdWrite, fdExcept, conns);
//	}
//
//	/*connectSocket = new ODSocket();
//	if (!connectSocket)
//	{
//	log("new error");
//	return;
//	}
//	if (connectSocket->Init() == -1)
//	{
//	log("Init error");
//	return;
//	}
//	if (!connectSocket->Create(AF_INET, SOCK_STREAM, 0))
//	{
//	log("Create error");
//	return;
//	}
//	if (!listenSocket->Accept(*connectSocket))
//	{
//	log("Accept error.");
//	return;
//	}*/
//}

//void OnlineGameScene::SendBroadcast()
//{
//	if (!udp)
//		return;
//	char* ip = NULL;
//	while (1)
//	{
//		if (connect)
//			break;
//		udp->SendTo("999", sizeof("999"), 0, ip, PORTA);
//		log("send999");
//#ifdef WIN32 
//		Sleep(1000);
//#else
//		usleep(1000);
//#endif
//	}
//}

//void OnlineGameScene::RecvBroadcast()
//{
//	Net::getInstance()->RecvBroadcast();
//	//while (1)
//	//{
//	//	if (!serverIP.empty())//
//	//	{
//	//		log("connect");
//	//		break;
//	//	}
//	//	char buf[BUFFERSIZE];
//	//	int len = udp->RecvFrom(buf, BUFFERSIZE, 0, serverIP, serverPort);
//	//	log(Value(len).asString().c_str());
//	//	if (len >= 3)
//	//	{
//	//		if (buf[0] == '9' && buf[1] == '9' && buf[2] == '9')
//	//		{
//	//			log("recv999");
//	//			log(serverIP.c_str());
//	//			m_mutex.lock();
//	//			str.push_back("999");
//	//			m_mutex.unlock();
//	//			return;
//	//		}
//	//	}
//	//	serverIP.clear();
//	//}
//}
//void OnlineGameScene::ResetFDSet(fd_set & fdRead, fd_set & fdWrite, fd_set & fdExcept, SOCKET sdListen, const ConnectionList & conns)
//{
//	FD_ZERO(&fdRead);
//	FD_ZERO(&fdWrite);
//	FD_ZERO(&fdExcept);
//	FD_SET(sdListen, &fdRead);
//	FD_SET(sdListen, &fdExcept);
//	ConnectionList::const_iterator it = conns.begin();
//	m_mutex1.lock();
//	for (; it != conns.end(); ++it)
//	{
//		Connection* pConn = *it;
//		if (pConn->recvBytes < BUFFERSIZE)
//		{
//			FD_SET(pConn->hSocket, &fdRead);
//		}
//		if (pConn->sendBytes > 0)
//		{
//			FD_SET(pConn->hSocket, &fdWrite);
//		}
//		FD_SET(pConn->hSocket, &fdExcept);
//	}
//	m_mutex1.unlock();
//}
//
//int OnlineGameScene::CheckAccept(const fd_set & fdRead, const fd_set & fdExcept, SOCKET sdListen, ConnectionList & conns)
//{
//	int lastErr = 0;
//	if (FD_ISSET(sdListen, &fdExcept))//sdListen 异常
//	{
//		int errlen = sizeof(lastErr);
//		getsockopt(sdListen, SOL_SOCKET, SO_ERROR, (char*)&lastErr, &errlen);
//		log("I/O error");
//		log(Value(lastErr).asString().c_str());
//		std::cout << "I/O error" << lastErr << std::endl;
//		return SOCKET_ERROR;
//	}
//	if (FD_ISSET(sdListen, &fdRead))//可以accept
//	{
//		log("accept ok");
//		if (conns.size() >= FD_SETSIZE - 1)
//		{
//			return 0;
//		}
//		sockaddr_in client;
//		int size = sizeof(sockaddr_in);
//		SOCKET sd = ::accept(sdListen, (sockaddr*)&client, &size);
//		lastErr = WSAGetLastError();
//		if (sd == INVALID_SOCKET && lastErr != WSAEWOULDBLOCK)
//		{
//			log("accept error");
//			std::cout << "accept error" << lastErr << std::endl;
//			return SOCKET_ERROR;
//		}
//		if (sd != INVALID_SOCKET)
//		{
//			u_long noBlock = 1;
//			if (ioctlsocket(sd, FIONBIO, &noBlock) == SOCKET_ERROR)
//			{
//				log("ioctlsocket error");
//				std::cout << "ioctlsocket error" << WSAGetLastError() << std::endl;
//				return SOCKET_ERROR;
//			}
//			conns.push_back(new Connection(sd));
//			connect = true;
//		}
//	}
//	return 0;
//}
//
//void OnlineGameScene::CheckConn(const fd_set & fdRead, const fd_set & fdWrite, const fd_set & fdExcept, ConnectionList & conns)
//{
//	log("chekconn");
//	ConnectionList::iterator it = conns.begin();
//	while (it != conns.end())
//	{
//		
//		Connection* pConn = *it;
//		bool ok = true;
//		if (FD_ISSET(pConn->hSocket, &fdExcept))
//		{
//			ok = false;
//			int lastErr;
//			int errlen = sizeof(lastErr);
//			getsockopt(pConn->hSocket, SOL_SOCKET, SO_ERROR, (char*)&lastErr, &errlen);
//			log("I/O error");
//			std::cout << "I/O error" << lastErr << std::endl;
//		}
//		else
//		{
//			if (FD_ISSET(pConn->hSocket, &fdRead))
//			{
//				ok = TryRead(pConn);
//			}
//			else if (FD_ISSET(pConn->hSocket, &fdWrite))
//			{
//				ok = TryWrite(pConn);
//			}
//		}
//		if (!ok)
//		{
//			closesocket(pConn->hSocket);
//			delete pConn;
//			it = conns.erase(it);
//		}
//		else
//			++it;
//	}
//}
//
//bool OnlineGameScene::TryRead(Connection * pConn)
//{
//	log("read enter");
//	int ret = recv(pConn->hSocket, pConn->recvBuffer + pConn->recvBytes, BUFFERSIZE - pConn->recvBytes, 0);
//	if (ret > 0)
//	{
//		log("read words;");
//		pConn->recvBytes += ret;
//		if (pConn->recvBytes >= 4)
//		{
//			log("recv 4words");
//			if (pConn->recvBuffer[0] == '1')//合法消息
//			{
//				if (pConn->recvBuffer[1] == '3' && pConn->recvBuffer[2] == '3')//first消息
//				{
//					myTurn = false;
//					flag = tag_x;
//					log("recv first");
//					m_mutex.lock();
//					str.push_back(pConn->recvBuffer);
//					m_mutex.unlock();
//				}
//				else if (pConn->recvBuffer[2] >= '0' && pConn->recvBuffer[2] < '3' && pConn->recvBuffer[1] >= '0' && pConn->recvBuffer[1] < '3')//对方下棋子消息
//				{
//					myTurn = true;
//					m_mutex.lock();
//					str.push_back(pConn->recvBuffer);
//					m_mutex.unlock();
//					auto nextFlag = flag == tag_o ? tag_x : tag_o;//获取对方棋子类型
//					data[pConn->recvBuffer[1] - '0'][pConn->recvBuffer[2] - '0'] = nextFlag;
//					log("recv keys");
//				}
//				pConn->recvBytes -= 4;
//				memmove(pConn->recvBuffer, pConn->recvBuffer + 4, pConn->recvBytes);
//			}
//		}
//		return true;
//	}
//	else if (ret == 0)
//	{
//		log("connection close by peer");
//		std::cout << "connection close by peer" << std::endl;
//		PassiveShutdown(pConn->hSocket, pConn->recvBuffer, pConn->recvBytes);
//		return false;
//	}
//	else
//	{
//		int lastErr = WSAGetLastError();
//		if (lastErr == WSAEWOULDBLOCK)
//		{
//			return true;
//		}
//		log("recv error");
//		std::cout << "recv error" << lastErr << std::endl;
//		return false;
//	}
//}
//
//bool OnlineGameScene::TryWrite(Connection * pConn)
//{
//	log("write enter");
//	//m_mutex.lock();
//	int ret = send(pConn->hSocket, pConn->sendBuffer, pConn->sendBytes, 0);
//	if (ret > 0)
//	{
//		log("write words;");
//		pConn->sendBytes -= ret;
//		if (pConn->sendBytes > 0)
//		{
//			memmove(pConn->sendBuffer, pConn->sendBuffer + ret, pConn->sendBytes);
//		}
//		return true;
//	}
//	else if (ret == 0)
//	{
//		log("connection close by peer");
//		std::cout << "connection close by peer" << std::endl;
//		PassiveShutdown(pConn->hSocket, pConn->sendBuffer, pConn->sendBytes);
//		return false;
//	}
//	else
//	{
//		int lastErr = WSAGetLastError();
//		if (lastErr == WSAEWOULDBLOCK)
//		{
//			return true;
//		}
//		log("send error");
//		std::cout << "send error" << lastErr << std::endl;
//		return false;
//	}
//}
//
//bool OnlineGameScene::PassiveShutdown(SOCKET sd, const char * buff, int len)
//{
//	if (buff != NULL && len > 0)
//	{
//		u_long noBlock = 1;
//		if (ioctlsocket(sd, FIONBIO, &noBlock) == SOCKET_ERROR)
//		{
//			log("ioctlsocket error");
//			std::cout << "ioctlsocket error" << WSAGetLastError() << std::endl;
//			return false;
//		}
//		int nSend = 0;
//		while (nSend < len)
//		{
//			int temp = send(sd, &buff[nSend], len - nSend, 0);
//			if (temp > 0)
//			{
//				nSend += temp;
//			}
//			else if (temp == SOCKET_ERROR)
//			{
//				log("send error");
//				std::cout << "send error" << WSAGetLastError() << std::endl;
//				return false;
//			}
//			else
//			{
//				log("Connection closed unexceptedly by peer");
//				std::cout << "Connection closed unexceptedly by peer" << std::endl;
//				break;
//			}
//		}
//	}
//	if (shutdown(sd, 1) == SOCKET_ERROR)
//	{
//		log("shutdown error");
//		std::cout << "shutdown error" << WSAGetLastError() << std::endl;
//		return false;
//	}
//	return true;
//}