#pragma once
#include<iostream>
#include<cmath>
#include<string>
#include<vector>
class Enemy;
class Tower {
private:
	//基础属性
	string type;
	int damage;
	double range;
	double firerate;
	int x, y;
	//炮塔位置
	int level;
	long cost;
	//升级价格
	string priority
		double cooldown;

public:Tower(string type, int x, int y, double cooldown, double range, int damage, double fireRate, int level, long cost, string priority) :type(type), x(x), y(y), damage(damage), range(range), firerate(firerate), level(level), cost(cost), cooldown(0), targetPriority(priority) {}
	  void setdamage(int a) {
		  damage = a;
	  }
	  int getdamage() {
		  return damage;
	  }
	  void setrange(double a) {
		  range = a;
	  }
	  double getrange() {
		  return range;
	  }
	  void setfirerate(double a) {
		  firerate = a;
	  }
	  double getfirerate() {
		  return firerate;
	  }
	  void setposition(int a, b) {
		  x = a;
		  y = b;
	  }
	  int getposition() {
		  return x, y;
	  }
	  //升级塔
	  void upgrade();
	  //  更新冷却时间（每一帧调用?!，用于递减冷却）
	  void updateCooldown(double deltaTime);
		  //  寻找攻击目标
		  Enemy* findTarget(const std::vector<Enemy*>& enemies);
		  

	bool canShoot();
	// 判断能否攻击 
	void shoot(vector<Monster>& monsters); // 发射炮弹 