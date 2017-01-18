#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__
#include "cocos2d.h"
#include <vector>
USING_NS_CC;
enum type
{
	tag_o = 0,
	tag_x,
	tag_none
};
enum gameState
{
	o_win = 0,
	x_win,
	none_win,
	undone
};
class GameScene : public Layer
{
public:
	GameScene();
	static Scene* createScene();
	virtual bool init();
	CREATE_FUNC(GameScene);
	virtual void onTouchEnded(Touch*, Event*);
	virtual bool onTouchBegan(Touch*, Event*);
	gameState getState();
	void restart();
private:
    std::vector<std::vector<type> > data;
	type flag;
};
#endif