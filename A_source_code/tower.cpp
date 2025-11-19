#include"tower.h"
void Tower::upgrade() {
	level++；
		if (level < 4) {
			damage += damage;
			range *= 1.5；
				firerate *= 0.9;
		}
		else {
			cout << "已是最高等级"；
		}

}
void Tower::updateCooldown(double deltaTime)
{
	if (cooldown > 0) {
		cooldown -= deltaTime;
		if (cooldown < 0) cooldown = 0;
	}
}
Tower::Enemy* findTarget(const std::vector<Enemy*>& enemies) {
	if (enemies.empty()) return nullptr;
	Enemy* target = nullptr;
	// 根据优先级筛选目标
	if (targetPriority == "closest") {
		// 找距离最近的敌人
		double minDist = range + 1; // 初始为超出范围的距离
		for (Enemy* enemy : enemies) {
			double dist = sqrt(pow(enemy.getX() - x, 2) + pow(enemy.getY() - y, 2));
			//计算距离
			if (dist <= range && dist < minDist) {
				minDist = dist;
				target = enemy;
			}
		}
	}
	else if (targetPriority == "lowest_health") {
		// 找血量最低的敌人
		int minHealth = INT_MAX;
		for (Enemy* enemy : enemies) {
			if (enemy.getHealth() < minHealth) {
				minHealth = enemy.getHealth();
				target = enemy;
			}
		}
	}
}
}
return target;
	}
	// 2. 攻击目标（需先调用findTarget确定目标）
	void attack(Enemy* target) {
		if (target == nullptr || cooldown > 0) return; // 无目标或冷却中，不攻击
		// 对敌人造成伤害
		target->takeDamage(damage);
		// 重置冷却时间
		cooldown = attackSpeed;
		// （可选）添加攻击特效逻辑，如播放动画、音效
		std::cout << "Tower [" << type << "] attacks Enemy! Damage: " << damage << std::endl;
	}

