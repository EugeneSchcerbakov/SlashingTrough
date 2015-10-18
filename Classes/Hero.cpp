//
//  Hero.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 11/07/15.
//
//

#include "Hero.h"
#include "DailyMissions.h"
#include "GameInfo.h"
#include "PlayerInfo.h"
#include "Store.h"
#include "Log.h"

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
    _health = gameinfo.getConstFloat("HERO_HEALTH_POINTS");
    _attackArea = gameinfo.getConstFloat("HERO_ATTACK_AREA");
    _damageUpValue = gameinfo.getConstFloat("HERO_DAMAGE_UP_VALUE");
    _damageUpKillPoints = gameinfo.getConstInt("HERO_DAMAGE_UP_KP");
    _staminaPoints = gameinfo.getConstFloat("HERO_STAMINA_POINTS");
    _staminaDrainTime = gameinfo.getConstFloat("HERO_STAMINA_DRAIN_TIME");
    _staminaDrainValue = gameinfo.getConstFloat("HERO_STAMINA_DRAIN_VALUE");
    _actionsSequenceMaxSize = gameinfo.getConstInt("HERO_ACTIONS_SEQUENCE_SIZE");
    
    float baseHealth = GameInfo::getInstance().getConstFloat("HERO_HEALTH_POINTS");
    
    PlayerInfo &player = PlayerInfo::getInstance();
    
    _equip = player.getEquipment();
    
    _damage = _equip.weapon->getDamage();
    _radius = _equip.weapon->getDistance();
    _health = baseHealth + _equip.armor->getExtraHealth();
    _maxHealth = _health;
    
    for (auto crystall : _equip.crystalls) {
        if (crystall) {
            crystall->onInit(this);
        }
    }
    
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
    
    for (auto crystall : _equip.crystalls) {
        if (crystall) {
            crystall->onUpdate(dt);
        }
    }
}

void Hero::onDamageReceived(float damage)
{
    sendEvent(Event("DamageReceived"));
    
    DailyTaskEvent event(Tracking::DamageReceived);
    event.data.setFloat("amount", damage);
    DailyMissions::getInstance().event(event);
}

void Hero::addHealth(float health, bool callDamageReceived)
{
    float actual = health;
    
    for (auto crystall : _equip.crystalls) {
        if (crystall) {
            actual = crystall->onDamage(actual);
        }
    }
    
    _health += actual;
    if (_health < 0.0f) {
        _health = 0.0f;
    }
    
    if (actual < 0.0f && callDamageReceived)
    {
        onDamageReceived(actual);
    }
    
    if (_health > _maxHealth) {
        _health = _maxHealth;
    }
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
                
                for (auto crystall : _equip.crystalls) {
                    if (crystall) {
                        crystall->onHit(entity);
                    }
                }
                
                if (!entity->isAlive())
                {
                    for (auto crystall : _equip.crystalls) {
                        if (crystall) {
                            crystall->onKill(entity);
                        }
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
    for (auto crystall : _equip.crystalls) {
        if (crystall) {
            crystall->onSwipeLeft();
        }
    }
}

void Hero::onSwipeRight()
{
    for (auto crystall : _equip.crystalls) {
        if (crystall) {
            crystall->onSwipeRight();
        }
    }
}

void Hero::onSwipeBack()
{
    for (auto crystall : _equip.crystalls) {
        if (crystall) {
            crystall->onSwipeBack();
        }
    }
}

void Hero::addAction(HeroAction *action)
{
    _actionSequence.push_back(action);
}

void Hero::addStamina(float stamina)
{
    _staminaPoints += stamina;
    
    float max = GameInfo::getInstance().getConstFloat("HERO_STAMINA_POINTS");
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

ItemWeapon* Hero::getWeapon() const
{
    return _equip.weapon;
}

ItemArmor* Hero::getArmor() const
{
    return _equip.armor;
}

PlayerInfo::Score Hero::getScore() const
{
    return _score;
}

Entities* Hero::getGoals()
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
