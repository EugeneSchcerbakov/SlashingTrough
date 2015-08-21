//
//  Hero.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 11/07/15.
//
//

#include "Hero.h"
#include "GameInfo.h"
#include "PlayerInfo.h"
#include "Store.h"

#include <math.h>

Hero::Hero()
: Entity(Entity::Type::HERO)
, _runningSpeed(0.0f)
, _leftSideBorder(0.0f)
, _rightSideBorder(0.0f)
, _staminaDrainTimeCounter(0.0f)
, _killPointToNextDamageUp(0)
, _goals(nullptr)
, _running(false)
{
    init();
}

void Hero::init()
{
    GameInfo &gameinfo = GameInfo::getInstance();
    
    _damage = 1.0f;
    _radius = 0.0f;
    _health = gameinfo.getFloat("HERO_HEALTH_POINTS");
    _attackArea = gameinfo.getFloat("HERO_ATTACK_AREA");
    _damageUpValue = gameinfo.getFloat("HERO_DAMAGE_UP_VALUE");
    _damageUpKillPoints = gameinfo.getInt("HERO_DAMAGE_UP_KP");
    _staminaPoints = gameinfo.getFloat("HERO_STAMINA_POINTS");
    _staminaDrainTime = gameinfo.getFloat("HERO_STAMINA_DRAIN_TIME");
    _staminaDrainValue = gameinfo.getFloat("HERO_STAMINA_DRAIN_VALUE");
    _actionsSequenceMaxSize = gameinfo.getInt("HERO_ACTIONS_SEQUENCE_SIZE");
    
    std::string weaponId = PlayerInfo::getInstance().getEquippedWeaponId();
    Equip::WeakPtr weapon = Store::getInstance().getItemById(weaponId);
    setWeapon(weapon);
    
    std::string armorId = PlayerInfo::getInstance().getEquippeArmorId();
    Equip::WeakPtr armor = Store::getInstance().getItemById(armorId);
    setArmor(armor);
    
    getWeapon()->featuresInit(this);
    getArmor()->featuresInit(this);
    
    flushScore();
}

void Hero::idleUpdate(float dt)
{
    if (_running) {
        _y += _runningSpeed * dt;
    }
    
    if (_actionSequence.size() > 0) {
        HeroAction *action = _actionSequence.front();
        if (!action->isFinished()) {
            if (!action->isStarted()) {
                action->start();
                Event event = action->getEvent();
                if (!event.is("")) {
                    sendEvent(event);
                }
            }
            action->update(dt);
        } else {
            delete action;
            _actionSequence.pop_front();
        }
    }
    
    _staminaDrainTimeCounter += dt;
    if (_staminaDrainTimeCounter >= _staminaDrainTime) {
        _staminaDrainTimeCounter = 0.0f;
        addStamina(-_staminaDrainValue);
    }
    
    if (_staminaPoints <= 0.0f && isAlive()) {
        _staminaPoints = 0.0f;
        kill();
    }
    
    getWeapon()->featuresUpdate(dt);
    getArmor()->featuresUpdate(dt);
}

void Hero::onDamageReceived()
{
    sendEvent(Event("DamageReceived"));
}

void Hero::attack()
{
    if (_goals) {
        float x1 = getPositionX();
        float y1 = getPositionY();
        float xface = 0.0f;
        float yface = 1.0f;
        for (Entity *entity : *_goals) {
            if (!entity || !entity->isAlive()) {
                continue;
            }
            float x2 = entity->getPositionX();
            float y2 = entity->getPositionY();
            
            float dx = x2 - x1;
            float dy = y2 - y1;
            
            float len = sqrtf(dx * dx + dy * dy);
            
            float n = len;
            n = 1.0f / n;
            float nx = dx * n;
            float ny = dy * n;
            
            float dotp = nx * xface + ny * yface;
            
            if (len <= _radius && dotp >= _attackArea) {
                entity->addHealth(-_damage);
                
                getWeapon()->featuresOnHit(entity);
                
                if (!entity->isAlive()) {
                    Reward *reward = dynamic_cast<Reward *>(entity);
                    if (reward) {
                        addKillsPoint(reward->getKillPoints());
                        addCoinsPoint(reward->getCoinPoints());
                        addStamina(reward->getStaminaPoints());
                        addScorePoint(reward->getScorePoints());
                        
                        getWeapon()->featuresOnKill(entity);
                    }
                }
            }
        }
    }
}

void Hero::flushScore()
{
    _score.coins = 0;
    _score.kills = 0;
    _score.score = 0;
}

void Hero::refreshGoals(Entities *entities)
{
    if (entities) {
        _goals = entities;
    }
}

void Hero::onSwipeLeft()
{
    getWeapon()->featuresOnSwipeLeft();
    getArmor()->featuresOnSwipeLeft();
}

void Hero::onSwipeRight()
{
    getWeapon()->featuresOnSwipeRight();
    getArmor()->featuresOnSwipeRight();
}

void Hero::onSwipeBack()
{
    getWeapon()->featuresOnSwipeBack();
    getArmor()->featuresOnSwipeBack();
}

void Hero::addAction(HeroAction *action)
{
    _actionSequence.push_back(action);
}

void Hero::addStamina(float stamina)
{
    _staminaPoints += stamina;
    
    float max = GameInfo::getInstance().getFloat("HERO_STAMINA_POINTS");
    if (_staminaPoints > max) {
        _staminaPoints = max;
    }
    if (_staminaPoints < 0.0f) {
        _staminaPoints = 0.0f;
    }
}

void Hero::addKillsPoint(int killsPoint)
{
    _score.kills += killsPoint;
    _killPointToNextDamageUp += killsPoint;
    if (_killPointToNextDamageUp >= _damageUpKillPoints) {
        _killPointToNextDamageUp = 0;
        _damage += _damageUpValue;
    }
}

void Hero::addCoinsPoint(int coinsPoint)
{
    _score.coins += coinsPoint;
}

void Hero::addScorePoint(int scorePoint)
{
    _score.score = scorePoint;
}

void Hero::setWeapon(Equip::WeakPtr weapon)
{
    if (weapon.expired()) {
        return;
    }
    
    _weapon = weapon;
    Equip::Ptr base = _weapon.lock();
    EquipWeapon *cast = EquipWeapon::cast(base);
    
    _damage = cast->getDamage();
    _radius = cast->getDistance();
}

void Hero::setArmor(Equip::WeakPtr armor)
{
    if (armor.expired()) {
        return;
    }
    
    _armor = armor;
    Equip::Ptr base = _armor.lock();
    EquipArmor *cast = EquipArmor::cast(base);
    
    float baseHealth = GameInfo::getInstance().getFloat("HERO_HEALTH_POINTS");
    
    _health = baseHealth + cast->getExtraHealth();
}

void Hero::setRunningSpeed(float speed)
{
    _runningSpeed = speed;
}

void Hero::setRunning(bool running)
{
    _running = running;
}

void Hero::setSideBorders(float left, float right)
{
    _leftSideBorder = left;
    _rightSideBorder = right;
}

float Hero::getSpeed() const
{
    return _runningSpeed;
}

float Hero::getStamina() const
{
    return _staminaPoints;
}

HeroAction* Hero::getLastAction() const
{
    if (!_actionSequence.empty()) {
        return _actionSequence.back();
    }
    return nullptr;
}

EquipWeapon* Hero::getWeapon() const
{
    if (!_weapon.expired()) {
        return EquipWeapon::cast(_weapon.lock());
    }
    return nullptr;
}

EquipArmor* Hero::getArmor() const
{
    if (!_armor.expired()) {
        return EquipArmor::cast(_armor.lock());
    }
    return nullptr;
}

PlayerInfo::Score Hero::getScore() const
{
    return _score;
}

const Entities* Hero::getGoals() const
{
    return _goals;
}

bool Hero::isActionsQueueFull() const
{
    return _actionSequence.size() >= (std::size_t)_actionsSequenceMaxSize;
}

bool Hero::isAbleToPerform(HeroAction *action)
{
    if (!action) {
        return false;
    }
    
    if (action->isType(HeroAction::Type::ATTACK_MOVE))
    {
        auto move = dynamic_cast<AttackAndMove *>(action);
        float x = move->getFinishX();
        return x < _rightSideBorder && x > _leftSideBorder;
    }
    else if (action->isType(HeroAction::Type::JUMPBACK))
    {
        return true;
    }
    else if (action->isType(HeroAction::Type::ATTACK))
    {
        return true;
    }
    
    return false;
}

bool Hero::isActionInQueue(const std::string &tag) const
{
    if (tag.empty()) {
        return false;
    }
    for (auto it = _actionSequence.begin(); it != _actionSequence.end(); ++it) {
        HeroAction *action = (*it);
        if (action && (*it)->isTag(tag)) {
            return true;
        }
    }
    return false;
}
