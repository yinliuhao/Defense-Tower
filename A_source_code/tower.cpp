#include"tower.h"
// 塔类型枚举（清晰区分不同塔，避免魔法数字）
enum TowerType {
	//请输入文本*
};
// 1. 放置塔到地图函数实现
bool Tower::put(GameMap& map, int posX, int posY) {
    // 校验1：塔是否已放置（避免重复放置）
    if (isPlaced) {
        cout << "错误：当前塔已放置在地图上，不可重复放置！" << endl;
        return false;
    }
    // 校验2：放置位置是否合法（调用地图接口，判断是否为可建造区域、未超出地图范围）
    if (!map.isValidBuildPos(posX, posY)) {
        cout << "错误：坐标（" << posX << "," << posY << "）不可放置塔（障碍物/超出范围）！" << endl;
        return false;
    }
    // 校验3：塔类型是否已设置（未设置类型则无建造成本，无法放置）
    if (towerType == static_cast<TowerType>(-1)) {
        cout << "错误：请先设置塔的种类，再进行放置！" << endl;
        return false;
    }

    // 执行放置逻辑：更新塔的坐标、标记为已放置
    this->x = posX;
    this->y = posY;
    this->isPlaced = true;
    this->isActive = true;  // 放置后激活塔
    // 通知地图：标记该位置已建造塔（更新地图格子状态）
    map.markBuildPos(posX, posY, true);
    cout << "成功：塔放置在坐标（" << posX << "," << posY << "）！" << endl;
    return true;
}

// 2. 设置塔种类函数实现
void Tower::setTowerType(TowerType type) {
    // 更新塔类型
    this->towerType = type;
    // 自动初始化该类型塔的基础属性
    initTowerAttr();

    // 打印设置结果
    string typeName = "";
    switch (type) {
    case TOWER_: typeName = "塔"; break;
    case TOWER_: typeName = "塔"; break;
    case TOWER_: typeName = "塔"; break;
    case TOWER_: typeName = "塔"; break;
    default: typeName = "未知塔"; break;
    }
    cout << "成功：塔种类设置为【" << typeName << "】，已自动初始化基础属性！" << endl;
}

// 3. 选择攻击对象函数实现
BaseEnemy* Tower::selectAttackTarget(const vector<BaseEnemy*>& enemies) {
    // 校验：塔是否已放置且激活（未放置/已摧毁则无法选目标）
    if (!isPlaced || !isActive) {
        cout << "警告：塔未放置或已摧毁，无法选择攻击目标！" << endl;
        return nullptr;
    }
    // 校验：是否有存活敌人
    if (enemies.empty()) {
        return nullptr;
    }

    BaseEnemy* target = nullptr;
    double minDistance = attackRange + 1;  // 初始距离设为超出攻击范围

    // 遍历所有敌人，筛选“在攻击范围内+距离最近”的目标（默认优先级）
    for (BaseEnemy* enemy : enemies) {
        // 跳过已死亡的敌人
        if (!enemy->isAlive()) {
            continue;
        }

        // 计算塔与敌人的直线距离（调用之前的两点间距离公式）
        double dx = enemy->getX() - this->x;
        double dy = enemy->getY() - this->y;
        double distance = sqrt(dx * dx + dy * dy);

        // 筛选：在攻击范围内，且距离比当前目标更近
        if (distance <= attackRange && distance < minDistance) {
            minDistance = distance;
            target = enemy;
        }
    }

    // 若找到目标，打印目标信息；否则返回nullptr
    if (target != nullptr) {
        cout << "成功：选中攻击目标，敌人ID：" << target->getId() << "，距离：" << minDistance << endl;
    }
    return target;
}

// 4. 初始化塔基础属性函数实现（根据塔种类赋值）
void Tower::initTowerAttr() {
    // 根据塔类型，赋值对应的基础属性（数值可根据游戏平衡调整）
    switch (towerType) {
    case TOWER_ARCHER:  // 弓箭塔：射速快、伤害中、范围中
        health = 200;
        buildCost = 100;
        damage = 25;
        attackRange = 18.0;
        attackSpeed = 1.5;  // 每1.5秒攻击一次
        break;
    case TOWER_CANNON:  // 炮塔：伤害高、射速慢、范围中、有击退
        health = 300;
        buildCost = 150;
        damage = 60;
        attackRange = 15.0;
        attackSpeed = 3.0;  // 每3秒攻击一次
        break;
    case TOWER_MAGE:    // 法师塔：伤害中、射速中、范围小、带减速
        health = 150;
        buildCost = 120;
        damage = 35;
        attackRange = 12.0;
        attackSpeed = 2.0;  // 每2秒攻击一次
        break;
    case TOWER_SUPPORT: // 辅助塔：无伤害、射速无、范围大、带增益
        health = 180;
        buildCost = 180;
        damage = 0;
        attackRange = 25.0;
        attackSpeed = 0.0;  // 无攻击，无需射速
        break;
    default:  // 未知类型，赋值默认属性
        health = 100;
        buildCost = 50;
        damage = 10;
        attackRange = 10.0;
        attackSpeed = 2.0;
        cout << "警告：未知塔类型，已赋值默认基础属性！" << endl;
        break;
    }
}

// 5. 铲除塔函数实现
bool Tower::remove(GameMap& map, int& currentGold) {
    // 校验：塔是否已放置（未放置则无法铲除）
    if (!isPlaced) {
        cout << "错误：塔未放置在地图上，无法铲除！" << endl;
        return false;
    }

    // 执行铲除逻辑：标记为未放置、未激活
    this->isPlaced = false;
    this->isActive = false;
    // 通知地图：恢复该位置为可建造区域
    map.markBuildPos(this->x, this->y, false);
    // 返还部分金币（50%建造成本，可调整）
    int refundGold = buildCost * 0.5;
    currentGold += refundGold;

    cout << "成功：塔已铲除！返还金币：" << refundGold << "，当前剩余金币：" << currentGold << endl;
    return true;
}

// 6. 选择设置子弹函数实现（利用友元关系）
bool Tower::selectBullet(int bulletType) {
    // 校验：塔是否已放置且激活（未放置则无需设置子弹）
    if (!isPlaced || !isActive) {
        cout << "错误：塔未放置或已摧毁，无法设置子弹！" << endl;
        return false;
    }
    // 校验：子弹类型是否合法（1-3为有效类型，可根据小组Bullet类扩展）
    if (bulletType < 1 || bulletType > 3) {
        cout << "错误：非法子弹类型！仅支持1（普通弹）、2（穿甲弹）、3（高爆弹）" << endl;
        return false;
    }

    // 若当前已有子弹，先释放资源（避免内存泄漏）
    if (currentBullet != nullptr) {
        delete currentBullet;
        currentBullet = nullptr;
    }

    // 创建对应类型的子弹对象（结合友元关系，设置子弹属性）
    currentBullet = new Bullet();
    switch (bulletType) {
    case 1:  // 普通弹：适配所有塔，伤害=塔基础伤害，速度中等
        currentBullet->bulletType = 1;
        currentBullet->damage = this->damage;  // 友元访问：直接赋值塔的伤害
        currentBullet->speed = 10.0;
        currentBullet->isArmorPiercing = false;
        cout << "成功：设置子弹类型为【普通弹】！" << endl;
        break;
    case 2:  // 穿甲弹：适配炮塔，伤害=塔基础伤害*1.2，速度快，可穿透
        currentBullet->bulletType = 2;
        currentBullet->damage = this->damage * 1.2;
        currentBullet->speed = 15.0;
        currentBullet->isArmorPiercing = true;
        cout << "成功：设置子弹类型为【穿甲弹】！" << endl;
        break;
    case 3:  // 高爆弹：适配炮塔，伤害=塔基础伤害*0.8，范围伤害，速度慢
        currentBullet->bulletType = 3;
        currentBullet->damage = this->damage * 0.8;
        currentBullet->speed = 8.0;
        currentBullet->explosionRange = 5.0;  // 友元访问：设置爆炸范围
        cout << "成功：设置子弹类型为【高爆弹】！" << endl;
        break;
    }

    return true;
}
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

