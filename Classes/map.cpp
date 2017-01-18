//#include"map.h"
//gameMap::gameMap()
//{
//	for(int i = 0; i < 3; i++)
//		for (int j = 0; j < 3; j++)
//		{
//			point temp(i, j);
//			m.insert(map<point, type>::value_type(temp, tag_none));
//		}
//}
//gameState gameMap::isWin()
//{
//	gameState winner = none_win;
//	for (int i = 0; i < 3; i++)
//	{
//		type temp = m[point(i, 0)];
//		int j = 1;
//		for (; j < 3; j++)
//		{
//			if (m[point(i, j)] != temp)
//				break;
//		}
//		if (j == 3)
//		{
//			winner = temp == tag_x ? x_win : o_win;
//			return winner;
//		}
//	}
//	for (int i = 0; i < 3; i++)
//	{
//		type temp = m[point(0, i)];
//		int j = 1;
//		for (; j < 3; j++)
//		{
//			if (m[point(j, i)] != temp)
//				break;
//		}
//		if (j == 3)
//		{
//			winner = temp == tag_x ? x_win : o_win;
//			return winner;
//		}
//	}
//	if (m[point(0, 0)] == m[point(1, 1)] && m[point(0, 0)] == m[point(2, 2)])
//	{
//		winner = m[point(0, 0)] == tag_x ? x_win: o_win;
//		return winner;
//	}
//	if (m[point(0, 2)] == m[point(1, 1)] && m[point(0, 2)] == m[point(2, 0)])
//	{
//		winner = m[point(0, 0)] == tag_x ? x_win : o_win;
//		return winner;
//	}
//	return winner;
//}
////Sprite* gameMap::set(point x, type t)
////{
////	m[x] = t;
////	auto sprite = Sprite::create("o.png");
////	sprite->setAnchorPoint(Vec2::ZERO);
////	sprite->setPosition(25, 165);
////	return sprite;
////}
