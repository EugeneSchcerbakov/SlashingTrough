//
//  Hero.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 11/07/15.
//
//

#include "Hero.h"
#include "GameInfo.h"
#include "SessionInfo.h"
#include "Store.h"

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
    _damageUpValue = gameinfo.getFloat("HERO_DAMAGE_UP_VALUE");
    _damageUpKillPoints = gameinfo.getInt("HERO_DAMAGE_UP_KP");
    _staminaPoints = gameinfo.getFloat("HERO_STAMINA_POINTS");
    _staminaDrainTime = gameinfo.getFloat("HERO_STAMINA_DRAIN_TIME");
    _staminaDrainValue = gameinfo.getFloat("HERO_STAMINA_DRAIN_VALUE");
    _actionsSequenceMaxSize = gameinfo.getInt("HERO_ACTIONS_SEQUENCE_SIZE");
    
    std::string weaponId = SessionInfo::getInstance().getEquippedWeaponId();
    Equip::WeakPtr weapon = Store::getInstance().getItemById(weaponId);
    setWeapon(weapon);
    
    for (auto ability : getWeapon()->abilities) {
        ability->init(this);
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
            _actionSequence.pop();
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
            float area = 0.85f;
            
            if (len <= _radius && dotp >= area) {
                entity->addHealth(-_damage);
                entity->onDamageReceived();
                
                for (auto ability : getWeapon()->abilities) {
                    ability->onHit(entity);
                }
                
                if (!entity->isAlive()) {
                    Reward *reward = dynamic_cast<Reward *>(entity);
                    if (reward) {
                        addKillsPoint(reward->getKillPoints());
                        addCoinsPoint(reward->getCoinPoints());
                        addStamina(reward->getStaminaPoints());
                        addScorePoint(reward->getScorePoints());
                        
                        for (auto ability : getWeapon()->abilities) {
                            ability->onKill(entity);
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

void Hero::addAction(HeroAction *action)
{
    _actionSequence.push(action);
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
    
    _damage = cast->damage;
    _radius = cast->distance;
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

SessionInfo::Score Hero::getScore() const
{
    return _score;
}

bool Hero::isActionsQueueFull() const
{
    return _actionSequence.size() >= _actionsSequenceMaxSize;
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
