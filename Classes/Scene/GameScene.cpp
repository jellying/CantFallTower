#include "GameScene.h"
#include "Enemy\Enemy.h"
Vector<TDPoint*> GameScene::allPoint;//保存所有路径转弯点

Scene * GameScene::createScene()
{
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = GameScene::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

bool GameScene::init()
{
	if (!Layer::init())
	{
		return false;
	}
	//添加背景图片
	auto spriteBG = Sprite::create("Map_Ground_02.jpg");
	addChild(spriteBG);
	spriteBG->setPosition(Vec2(480, 320));

	//添加地图文件
	auto Map = TMXTiledMap::create("map_0.tmx");
	addChild(Map);
	Map->setTag(0);//将地图文件Tag设置为0

	//加载敌人行走路径点
	initAllPoints();
	//设置敌人数量
	enemyMaxCount = 20;
	enemyCreated = 0;
	//产生一大波怪物
	schedule(schedule_selector(GameScene::EnemyCreat),1 );

	return true;
}


//加载敌人行走路径点
void GameScene::initAllPoints()
{
	//得到tmx地图
	TMXTiledMap* ourMap = (TMXTiledMap*)this->getChildByTag(0);
	auto Group = ourMap->getObjectGroup("Obj1");//获取对象层数据
	auto Objs = Group->getObjects();

	for (auto &eachObj : Objs)
	{
		ValueMap& dict = eachObj.asValueMap();
		float x = dict["x"].asFloat();
		float y = dict["y"].asFloat();
		//将各个点数据提取并生成TDPoint
		TDPoint * newPoint = TDPoint::createPoint(x, y);
		//将新生成的TDPoint加入栈
		allPoint.pushBack(newPoint);
	}
}


void GameScene::EnemyCreat(float dt)
{
	if (enemyCreated < enemyMaxCount)
	{
		//产生的敌人未达到最大数量则继续产生
		++enemyCreated;
		auto newEnemy = Enemy::creatEnemy(1);
		addChild(newEnemy);
		enemyList.pushBack(newEnemy);
	}
	clearRemovedEnemyFromList();
	if (enemyList.size() == 0)
	{
		//当产生的怪物全被消灭重置怪物生成
		enemyCreated = 0;
	}
}

void GameScene::judgeEntityBounding(Entity* entity, Enemy* singleEnemy)
{
	clearRemovedEnemyFromList();

	if (singleEnemy!=nullptr)
	{
		if (singleEnemy->isDead() || !enemyList.contains(singleEnemy))
			return;
		entity->judgeSingleEnemy(singleEnemy);
		return;
	}

	for (Enemy* enemy : enemyList)
	{
		entity->judgeSingleEnemy(enemy);
	}

}

void GameScene::clearRemovedEnemyFromList()
{
	for (auto iter = enemyList.begin(); iter != enemyList.end();)
	{
		if ((*iter)->getParent()==nullptr)
		{
			iter = enemyList.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}
