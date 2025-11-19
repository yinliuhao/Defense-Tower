#include <iostream>
#include"monster.h"
#include<algorithm>



class path {
private:
	std::vector<Vector2>waypoints;
public:
	void addWaypoint(const Vector2& point) {
		waypoints.push_back(point);
	}
};
monster::monster(const std::string& monstername, const Vector2& startPos, int health, float speed, int armor, int goldreward, int expreward)
	:name(monstername),
	uniqueID(nextID++),
	position(startPos),
	size(30, 30),
	maxHealth(health), currentHealth(health),
	baseSpeed(speed), currentSpeed(speed), speedModifier(1.0f),
	baseArmor(armor),
	rewardGold(rewardGold), rewardExp(rewardExp),
	currentWaypoint(0), progress(0.0f), reachedEnd(false),

	primaryState(PrimaryState::NORMAL),
	activeStateFlags(static_cast<StateFlags>(StateFlag::NONE)),
	previousStateFlags(static_cast<StateFlags>(StateFlag::NONE)),
	isMovementLocked(false),
	globalStateTimer(0), damageFlashTimer(0), poisonDamageTimer(0)
	//originalColor(color),currentColor(color),damageFlashColor(color),
{};

monster::~monster() {

}

void monster::update(float deltatime)
{
	if (primaryState == PrimaryState::DEAD) { return; }
	updateStatusEffects(deltatime);
	updateStateFlagTimers(deltatime);
	updateMovement(deltatime);
	updateStateSpecificLogic(deltatime);
	updateHealthBar();
	if (damageFlashTimer > 0) {
		damageFlashTimer -= deltatime;
		if (damageFlashTimer <= 0) { resetColor(); }
	}
	if (reachedEnd && primaryState != PrimaryState::REACHED_END) {
		onReachEnd;
	}
}
void monster::setPath(const std::vector<Vector2>& newpath)
{
	path = newpath;
	currentWaypoint = 1;
	reachedEnd = false;
	if (!path.empty()) {
		position = path[0];
	}
	reachedEnd = false;
}


void monster::updateMovement(float deltatime)
{
	if (path.size() < 2 || currentWaypoint >= path.size()) {
		reachedEnd = true;
		return;
	}
	Vector2 target = path[currentWaypoint];//获取当前目标点
	Vector2 direction = (target - position).normalized();//计算移动方向
	float distanceToTarget = position.distanceTo(target);//计算到目标点的距离
	float effectiveSpeed = currentSpeed * speedModifier;
	float moveDistance = effectiveSpeed * deltatime;

	if (moveDistance >= distanceToTarget) {
		position = target;
		currentWaypoint++;
		if (currentWaypoint >= path.size()) {
			reachedEnd = true;
			return;
		}
		float extraDistance = moveDistance - distanceToTarget;
		if (extraDistance > 0) {
			updateMovement(extraDistance / effectiveSpeed);
		}
	}
	else {
		position = position + direction * moveDistance;
	}
}
void monster::setStateFlag(StateFlag flag, bool active) {
	StateFlags oldFlags = activeStateFlags;
}
void monster::applySpeedModifier(float modifier)
{
	speedModifier *= modifier;
	currentSpeed = baseSpeed * speedModifier;
}
void monster::removeSpeedModifier(float modifier)
{
	speedModifier /= modifier;
	currentSpeed = baseSpeed * speedModifier;
}
void monster::takeDamage(int damage,bool isPhysical)
{
	if (!isAlive) {
		return;
	}
	int actualDamage = damage;
	if (isPhysical) {
		actualDamage = std::max(1, damage - baseArmor);//物理伤害计算护甲
	}
	else { actualDamage = damage; }//魔法伤害无视护甲
	
	currentHealth -= actualDamage;
	onDamageTaken(actualDamage);
	if (currentHealth <= 0) {
		currentHealth = 0;
		handleDeath();
	}
}

void monster::applyEffect(std::unique_ptr<StatusEffect>effect)
{
	EffectType effectType = effect->getType();
	StateFlag correspondingFlag = effectTypeToStateFlag(effectType);
	if (correspondingFlag != StateFlag::NONE) {
		setStateFlag(correspondingFlag, true);
	}
	effect->onApply(this);
	activeEffects.push_back(std::move(effect));
	//effectCounters[effectType]++
}
bool monster::hasStateFlag(StateFlag flag)const {
	return(activeStateFlags & static_cast<StateFlags>(flag)) != 0;
}
bool monster::canMove()const {
	return!isMovementLocked && primaryState != PrimaryState::IMMOBILIZED && primaryState != PrimaryState::DEAD && isAlive();
}
void monster::lockMovement(bool locked) {
	isMovementLocked = locked;
}
//void monster::setColor(const sf::Color& color) {
//	currentColor = color;
//	shape.setFillcolor(currentColor);
//}

void monster::onDamageTaken(int damage) {
	damageFlashTimer = 0.1f;
	//shape>setFillColor()
}
void monster::handleDeath() {
	/*player->addGold(rewardGold);
	player->addEXp(rewardExp);*/
	primaryState = PrimaryState::DEAD;
}
void monster::onReachEnd()
{
	//player->takedamage(1);
	primaryState = PrimaryState::REACHED_END;
}
void monster::updateStateModifiers()
{
	speedModifier = 1.0f;
	if (hasStateFlag(StateFlag::SLOWED)) {
		speedModifier *= 0.5f;
	}
	if (hasStateFlag(StateFlag::FROZEN)) {
		speedModifier *= 0.0f;
	}
	speedModifier = std::max(0.0f, std::min(speedModifier, 2.0f));
	currentSpeed = baseSpeed *= speedModifier;
}
void monster::updateStatusEffects(float deltatime)
{
	for (auto& effect : activeEffects) {
		effect->updateTime(deltatime);
		effect->onUpdate(this, deltatime);
	}
	cleanupExpiredEffect();
	updateStateModifiers();
}

void monster::updateStateSpecificLogic(float deltatime)
{
	if (hasStateFlag(StateFlag::POISONED)) {
		updatePoisonState(deltatime);
	}
	if (hasStateFlag(StateFlag::FROZEN)) {
		updateFrozenState(deltatime);
	}
	if (hasStateFlag(StateFlag::SLOWED)) {
		updateSlowedState(deltatime);
	}
}

void monster::updatePrimaryState()
{
	PrimaryState newPrimaryState = PrimaryState::NORMAL;
	if (hasStateFlag(StateFlag::FROZEN)) {
		newPrimaryState = PrimaryState::IMMOBILIZED;
	}
	if (!isAlive()) {
		newPrimaryState = PrimaryState::DEAD;
	}
	if (reachedEnd) {
		newPrimaryState = PrimaryState::REACHED_END;
	}
	if (newPrimaryState != primaryState) {
		
		primaryState = newPrimaryState;
	}
}
//void monster::cleanupExpiredEffect()
//{
//	auto it = activeEffects.begin();
//	while (it != activeEffects.end()) {
//		if ((*it)->isExpired()) {
//			EffectType expiredtype = (*it).getType();
//		}
//	}
//}

StateFlag monster::effectTypeToStateFlag(EffectType type)const {
	switch (type) {
	case EffectType::SLOW:return StateFlag::SLOWED;
	case EffectType::FREEZE:return StateFlag::FROZEN;
	case EffectType::POISON:return StateFlag::POISONED;
	default:return StateFlag::NONE;
	}
}

void SlowEffect::onApply(monster* target) {};
void SlowEffect::onUpdate(monster* target, float deltatime) {};
void SlowEffect::onRemove(monster* target) {};
void FreezeEffect::onApply(monster* target) {};
void FreezeEffect::onUpdate(monster* target, float deltatime) {};
void FreezeEffect::onRemove(monster* target) {};
void PoisonEffect::onApply(monster* target) {};
void PoisonEffect::onUpdate(monster* target, float deltatime) {};
void PoisonEffect::onRemove(monster* target) {};
