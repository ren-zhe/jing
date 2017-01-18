#include"GameScene.h"
#include"HelloWorldScene.h"
GameScene::GameScene()
{
	std::vector<type> temp(3);
	for (std::vector<type>::iterator it = temp.begin(); it != temp.end(); it++)
		*it = tag_none;
	for (int i = 0; i < 3; i++)
		data.push_back(temp);
	flag = tag_o;
}
Scene* GameScene::createScene()
{
	Scene* scene = Scene::create();
	Layer* layer = GameScene::create();
	scene->addChild(layer);
	return scene;
}
bool GameScene::init()
{
	if (!Layer::init())
	{
		return false;
	}

	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	Size visibleSize = Director::getInstance()->getVisibleSize();
	auto bg = Sprite::create("backgroundGame.png");
	bg->setPosition(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2);
	addChild(bg);
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
	listener->onTouchBegan = CC_CALLBACK_2(GameScene::onTouchBegan, this);
	listener->onTouchEnded = CC_CALLBACK_2(GameScene::onTouchEnded, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
	return true;

}
gameState GameScene::getState()
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
			winner = data.at(0).at(0) == tag_x ? x_win: o_win;
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
void GameScene::restart()
{
	auto labelRestart = Label::createWithSystemFont("Restart", "Arial", 20);
	auto menuItemRestart = MenuItemLabel::create(labelRestart, [](Ref* pSender) {
		auto scene = GameScene::createScene();
		auto sc = TransitionFade::create(1.0f, scene);
		Director::getInstance()->replaceScene(sc);
	});
	auto labelExit = Label::createWithSystemFont("Exit", "Arial", 20);
	auto menuItemExit = MenuItemLabel::create(labelExit, [](Ref* pSender) {
		auto scene = HelloWorld::createScene();
		auto sc = TransitionFade::create(1.0f, scene);
		Director::getInstance()->replaceScene(sc);
	});
	auto menu = Menu::create(menuItemRestart, menuItemExit, NULL);
	menu->alignItemsHorizontallyWithPadding(100);
	menu->setPosition(Director::getInstance()->getVisibleSize().width / 2, Director::getInstance()->getVisibleSize().height / 2);
	addChild(menu);
}
void GameScene::onTouchEnded(Touch* touch, Event* event)
{
	Vec2 t = touch->getLocation();
	int i = (t.x - 15) / 110;
	int j = (Director::getInstance()->getVisibleSize().height - t.y - 130) / 110;
	if (i >= 0 && i < 3 && j >= 0 && j < 3 &&data[i][j] == tag_none)
	{
		log("ok");
		Sprite* flagSprite;
		if (flag == tag_o)
		{
			flagSprite = Sprite::create("o.png");
		}
		else
		{
			flagSprite = Sprite::create("x.png");
		}
		data.at(i).at(j) = flag;
		this->removeChildByTag(flag);
		flag = flag == tag_o ? tag_x : tag_o;
		flagSprite->setAnchorPoint(Vec2(0, 1));
		flagSprite->setPosition(i * 110 + 15, Director::getInstance()->getVisibleSize().height -(110 *j + 130));
		addChild(flagSprite);
		Size visibleSize = Director::getInstance()->getVisibleSize();
		Sprite* f;
		if (flag == tag_o)
		{
			f = Sprite::create("o.png");
		}
		else
		{
			f = Sprite::create("x.png");	
		}
		f->setTag(flag);
		f->setScale(0.3);
		f->setPosition(visibleSize.width / 2, visibleSize.height / 12);
		addChild(f);
		gameState state = getState();
		if (state != undone)
		{
			Sprite* sprite;
			switch (state)
			{
			case o_win:
				sprite = Sprite::create("o_win.png");
				break;
			case x_win:
				sprite = Sprite::create("x_win.png");
				break;
			case none_win:
				sprite = Sprite::create("none_win.png");
				break;
			}
			sprite->setPosition(visibleSize.width / 2, visibleSize.height / 2);
			addChild(sprite);
			restart();
		}
	}
}
 bool GameScene::onTouchBegan(Touch*, Event*)
{
	 gameState  state = getState();
	 if (state == undone)
		 return true;
	 else
		 return false;
}