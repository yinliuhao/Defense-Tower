#pragma once
#include<iostream>
//#include<SFML/graphic.cpp>
#include<vector>
#include<memory>
#include<map>
#include<cmath>


struct Vector2
{
	float x, y;
	Vector2(float x = 0, float y = 0) :x(x), y(y) {}
	float distanceTo(const Vector2& other)const {
		float dx = other.x - x;float dy = other.y - y;
		return std::sqrt(dx * dx + dy * dy);
	}
	Vector2 normalized()const {
		float len = std::sqrt(x * x + y * y);
		return len > 0 ? Vector2(x / len, y / len) : *this;
	}
	Vector2 operator+(const Vector2& other) const { return Vector2(x + other.x, y + other.y); }
	Vector2 operator-(const Vector2& other)const { return Vector2(x - other.x, y - other.y); }
	Vector2 operator*(float scalar)const { return Vector2(x * scalar, y * scalar); }
};

enum class StateFlag {
	NONE=0,
	SLOWED=1<<0,
	FROZEN=1<<1,
	POISONED=1<<2,
};
using StateFlags = uint32_t;
//主要状态枚举{互斥状态）
enum class PrimaryState {
	NORMAL,         //正常移动
	IMMOBILIZED,    //无法移动
	DEAD,           //死亡
	REACHED_END     //到达终点
};
enum class EffectType {
	NONE,
	SLOW,
	FREEZE,
	POISON,
};
//效果基类
class StatusEffect {
protected:
	EffectType type;//效果种类
	float duration;//持续时间
	float elapsedTime;//已过时间
public:
	StatusEffect(EffectType type, float duration) :
		type(type), duration(duration), elapsedTime(0) {
	}
	virtual~StatusEffect() = default;
	virtual void onApply(class monster* target) = 0;//效果应用时调e(用
	virtual void onUpdate(class monster* target, float deltatime) = 0;//效果更新时调用
	virtual void onRemove(class monster* target) = 0;//效果移除时调用
	virtual std::unique_ptr<StatusEffect>clone()const = 0;

	EffectType getType()const { return type; }
	float getDuration()const { return duration; }
	float getElapsedTime()const { return elapsedTime; }
	bool isExpired()const { return elapsedTime >= duration; }//检查效果是否过期
	void updateTime(float deltatime) { elapsedTime += deltatime; }
	
};
//减速效果类
class SlowEffect :public StatusEffect {
private:
	float speedReduction;//速度降低比例(0~1）
public:
	SlowEffect(float duration,float reduction):
		StatusEffect(EffectType::SLOW,duration),speedReduction(reduction){ }
	void onApply(monster* target)override;
	void onUpdate(monster* target, float deltatime)override;
	void onRemove(monster* target)override;
	std::unique_ptr<StatusEffect>clone()const override
	{
		return std::make_unique<SlowEffect>(*this);
	}
};
//冻结效果类
class FreezeEffect :public StatusEffect {
public:
	FreezeEffect(float duration) :
		StatusEffect(EffectType::FREEZE, duration) {
	}
	void onApply(monster* target)override;
	void onUpdate(monster* target, float deltatime)override;
	void onRemove(monster* target)override;
	std::unique_ptr<StatusEffect>clone()const override
	{
		return std::make_unique<FreezeEffect>(*this);
	}
};
//中毒效果类
class PoisonEffect :public StatusEffect {
private:
	int damagePerSecond;//每秒伤害
	float damageAccumulator;//伤害累计器
public:
	PoisonEffect(float duration,int dps) :
		StatusEffect(EffectType::POISON, duration),damagePerSecond(dps),damageAccumulator(0) {
	}
	void onApply(monster* target)override;
	void onUpdate(monster* target, float deltatime)override;
	void onRemove(monster* target)override;
	std::unique_ptr<StatusEffect>clone()const override
	{
		return std::make_unique<PoisonEffect>(*this);
	}
};
//--------------------------怪物主类——————————————
class monster {
private:
	//---基础属性---
	std::string name;//名字
	int uniqueID;//标识符
	static int nextID;//下一个可用ID
	int maxHealth;
	int currentHealth;
	int baseArmor;//护甲
	float baseSpeed;
	float currentSpeed;
	int rewardGold;//击败掉落金钱
	int rewardExp;//击败掉落经验值
	//---移动相关---
	Vector2 position;//当前位置坐标
	Vector2 size;//怪物尺寸
	std::vector<Vector2>path;
	int currentWaypoint;//目标路径点索引
	float progress;
	bool reachedEnd;//是否到达终点
	//---状态相关---
	PrimaryState primaryState;
	StateFlags activeStateFlags;
	StateFlags previousStateFlags;
	std::vector<std::unique_ptr<StatusEffect>>activeEffects;
	std::map<StateFlags, float>stateFlagTimers;
	float speedModifier;
	bool isMovementLocked;//移动是否被锁定
	bool isActionlocked;
	//---计时器相关---
	float globalStateTimer;
	float damageFlashTimer;
	float poisonDamageTimer;
	//---图形相关---
	/*sf::RectangleShape shape;//怪物形状
	sf::RectangleShape healthBar;//血条
	sf::Color originalColor;//原始颜色
	sf::Color currentColor;//当前颜色
	sf::Color damageFlashColor;//受伤闪烁颜色*/
public:
	monster(const std::string& monstername, const Vector2& startPos, int health, float speed, int armor, int goldReward, int expReward);
	~monster();
	//核心更新函数
	void update(float deltatime);
	//---移动相关---
	void setPath(const std::vector<Vector2>& newpath);//设置移动路径
	bool hasReachedEnd()const { return reachedEnd; }//检查是否到终点
	//---战斗相关---
	void takeDamage(int damage, bool isPhysical = true);//受伤
	void applyEffect(std::unique_ptr<StatusEffect>effect);//应用效果
	bool isAlive()const { return currentHealth > 0; }//检查是否存活
	bool hasStateFlag(StateFlag flag)const;//是否有指定状态
	bool canMove()const;
	//---获取器相关---
	Vector2 getPosition()const { return position; }
	std::string getName()const { return name; }
	int getID()const { return uniqueID; }
	float getRewardGold()const { return rewardGold; }
	float getRewardExp()const { return rewardExp; }
	//---状态工具相关---
	void lockMovement(bool locked);//移动锁定/解锁
	void applySpeedModifier(float modifier);//应用速度修饰
	void removeSpeedModifier(float modifier);//移除速度修饰
	//void setColor(const sf::Color& color);//设置颜色
	void resetColor();
private:
	//---初始化---
	void initializeStateFlagTimers();
	void initializeGraphic();//初始化图形
	//---更新相关---
	void updateStatusEffects(float deltatime);
	void updateStateFlagTimers(float deltatime);
	void updateMovement(float deltatime);//更新移动
	void updateStateSpecificLogic(float deltatime);
	void updateHealthBar();
	//---状态相关---
	void setStateFlag(StateFlag flag, bool active);
	void onStateFlagsChanged(StateFlags oldFlags, StateFlags newFlags);
	void onStateFlagsAdded(StateFlags addedFlags);
	void onStateFlagsRemoved(StateFlags removedFlags);
	void updatePrimaryState();
	
	//---具体状态相关---
	void onSlowedAdded();
	void onSlowedRemoved();
	void updateSlowedState(float deltatime);
	void onFrozenedAdded();
	void onFrozenRemoved();
	void updateFrozenState(float deltatime);
	void onPoisonAdded();
	void onPoisonRemoved();
	void updatePoisonState(float deltatime);
	//---伤害处理---
	void applyPoisonDamage();
	void onDamageTaken(int damage);//受伤处理
	void handleDeath();//死亡处理
	void onReachEnd();//到达终点处理
	
	void updateStateModifiers();
	void cleanupExpiredEffect();
	StateFlag effectTypeToStateFlag(EffectType type)const;
};


