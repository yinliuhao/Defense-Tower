#pragma once
#include<iostream>
#include<cmath>
#include<string>
#include<vector>
enum TowerType {

};
class Monster;
class Map;
class Tower {
	friend class Bullet;
private:
	// 塔基础属性（与initTowerAttr函数中的属性对应）
	TowerType towerType;       // 塔的种类
	int health;                // 塔的血量
	int buildCost;             // 安置塔所需金币
	int damage;                // 塔的基础伤害
	double attackRange;        // 塔的攻击范围（半径）
	double attackSpeed;        // 塔的射速（攻击间隔，秒/次）
	int x, y;                  // 塔在地图上的坐标
	bool isPlaced;             // 塔是否已放置到地图
	bool isActive;             // 塔是否激活（未摧毁）
	Bullet* currentBullet;     // 当前塔使用的子弹（关联友元类）

public:
	//升级塔
	void upgrade();
	//  更新冷却时间（是否采用？）
	void updateCooldown(double deltaTime);
	//  寻找攻击目标
	Monster* selectAttackTarget(const vector<Monster*>& enemies);
	//设置塔种类
	void setTowerType(TowerType type);
	//初始化塔的基本属性
	void initTowerAttr();
	//放置塔
	bool put(Map& map, int posX, int posY)
		//铲除塔
		bool remove(Map& map, int& currentGold)
}
 