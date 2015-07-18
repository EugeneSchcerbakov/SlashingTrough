//
//  Enemy.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 12/07/15.
//
//

#include "Enemy.h"
#include "Hero.h"
#include "Field.h"
#include "Projectile.h"

#include <math.h>

Enemy::Enemy(const GameInfo::EnemyType &type, Field *field)
: Entity(Entity::Type::ENEMY)
, Reward(type.killPoints, type.goldPointsReward,
         type.staminaDrainPoints, type.scorePointsReward)
, _field(field)
, _state(State::IDLE)
, _nextState(State::IDLE)
, _rangeAttackTimer(0.0f)
, _melleAttackTimer(0.0f)
, _melleDuration(0.35f)
{
    init(type);
}

void Enemy::idleUpdate(float dt)
{
    if (_stateTimer > 0.0f) {
        _stateTimer -= dt;
        if (_stateTimer <= 0.0f) {
            _stateTimer = 0.0;
            _state = _nextState;
        }
    }
    
    if (_goal)
    {
        // collision
        float dx = _goal->getPositionX() - _x;
        float dy = _goal->getPositionY() - _y;
        float len = sqrtf(dx * dx + dy * dy);
        
        if (len < _radius && _goal->getPositionY() < _y)
        {
            _goal->addHealth(-_damage);
            if (_goal->isType(Entity::Type::HERO)) {
                Hero *hero = dynamic_cast<Hero *>(_goal);
                hero->addStamina(-_staminaDrainPoints);
            }
            kill();
        }
        
        if (_goal->isType(Entity::Type::HERO))
        {
            auto hero = dynamic_cast<Hero *>(_goal);
            processRangeAttack(hero, dt, len);
            processMelleAttack(hero, dt, len);
        }
    }
}

void Enemy::processRangeAttack(Hero *hero, float dt, float len)
{
    if (!_rangeAttack.allowed) {
        return;
    }
    
    if (_rangeAttackTimer > 0.0f) {
        _rangeAttackTimer -= dt;
    }
    
    if (_rangeAttackTimer <= 0.0f && len < _rangeAttack.distance)
    {
        // calculate aniticipation
        
        float pt = len / _rangeAttack.projectile.speed * 0.5f;
        float px = hero->getPositionX() + pt * hero->getSpeed() * 0.0f;
        float py = hero->getPositionY() + pt * hero->getSpeed() * 1.0f;
        float dx = px - _x;
        float dy = py - _y;
        float plen = sqrtf(dx * dx + dy * dy);
        
        float n = plen;
        n = 1.0f / n;
        float nx = dx * n;
        float ny = dy * n;
        
        Projectile *projectile = nullptr;
        projectile = new Projectile(_rangeAttack.projectile, _x, _y, nx, ny);
        projectile->setGoal(_goal);
        _field->addEntity(projectile);
        
        _rangeAttackTimer = _rangeAttack.recoveryTime;
        if (_rangeAttackTimer <= 0.0f) {
            _rangeAttack.allowed = false;
        }
    }
}

void Enemy::processMelleAttack(Hero *hero, float dt, float len)
{
    if (!_melleAttack.allowed) {
        return;
    }
    
    if (_melleAttackTimer > 0.0f) {
        _melleAttackTimer -= dt;
    }
    
    float highlightRange = _melleAttack.showHighlightRange;
    
    if (_melleAttackTimer <= 0.0f && isState(State::IDLE) && len < highlightRange) {
        _state = State::MELLE_PREPEAR;
        _nextState = State::MELLE_ATTACK_BEGIN;
        _melleAreaCenterX = _x;
        _melleAreaCenterY = _y;
        const float offset = 100.0f;
        switch (_melleAttack.condition) {
            case GameInfo::EnemyType::Melle::SAME_LINE:
                _melleAreaCenterY -= _melleAreaHeight * 0.5f + offset;
                break;
            case GameInfo::EnemyType::Melle::LEFT_LINE:
                _melleAreaCenterX -= _melleAreaWidth * 0.5f + offset;
                break;
            case GameInfo::EnemyType::Melle::RIGHT_LINE:
                _melleAreaCenterX += _melleAreaWidth * 0.5f + offset;
                break;
            default:
                break;
        }
        const float squareSize = GameInfo::getInstance().getFloat("SQUARE_SIZE");
        if (_melleAreaCenterX < 0.0f || _melleAreaCenterX > (squareSize * 3.0f)) {
            _melleAttack.allowed = false;
            return;
        }
        Event e("ShowMelleHighlight");
        e.variables.setFloat("ShowTime", _melleAttack.showHighlightTime);
        sendEvent(e);
        _melleAttacked = false;
    } else if (isState(State::MELLE_ATTACK_BEGIN) && _nextState != State::MELLE_ATTACK_END) {
        _nextState = State::MELLE_ATTACK_END;
        _stateTimer = _melleDuration;
        Event e("ShowMelleAttack");
        e.variables.setFloat("ShowTime", _melleDuration);
        sendEvent(e);
    } else if (isState(State::MELLE_ATTACK_END)) {
        _state = State::IDLE;
        _nextState = State::IDLE;
        _melleAttackTimer = _melleAttack.recoveryTime;
        if (_melleAttackTimer <= 0.0f) {
            _melleAttack.allowed = false;
        }
    }
    
    if (isState(State::MELLE_PREPEAR)) {
        float x = hero->getPositionX();
        float y = hero->getPositionY();
        if (checkMelleZone(x, y)) {
            _state = State::MELLE_ATTACK_BEGIN;
        }
    }
    
    if (isState(State::MELLE_ATTACK_BEGIN) && _stateTimer >= _melleDuration * 0.5 && !_melleAttacked)
    {
        float x = hero->getPositionX();
        float y = hero->getPositionY();
        if (checkMelleZone(x, y)) {
            hero->addHealth(-_melleAttack.dmgHealth);
            hero->addStamina(-_melleAttack.dmgStamina);
            _melleAttacked = true;
        }
    }
}

void Enemy::onDamageReceived()
{
    Event event("DamageReceived");
    sendEvent(event);
}

float Enemy::getMelleAreaCenterX() const
{
    return _melleAreaCenterX;
}

float Enemy::getMelleAreaCenterY() const
{
    return _melleAreaCenterY;
}

float Enemy::getMelleAreaWidth() const
{
    return _melleAreaWidth;
}

float Enemy::getMelleAreaHeight() const
{
    return _melleAreaHeight;
}

bool Enemy::isState(Enemy::State state) const
{
    return _state == state;
}

bool Enemy::isMelleType(GameInfo::EnemyType::Melle type) const
{
    return _melleAttack.allowed ? type == _melleAttack.condition : false;
}

void Enemy::init(const GameInfo::EnemyType &type)
{
    _health = type.health;
    _damage = type.damage;
    _sprite = type.sprite;
    _radius = 150.0f;
    _staminaDrainPoints = type.staminaDrainPoints;
    _rangeAttack = type.rangeAttack;
    _melleAttack = type.melleAttack;
    
    const float squareSize = GameInfo::getInstance().getFloat("SQUARE_SIZE");
    
    _melleAreaCenterX = _x;
    _melleAreaCenterY = _y;
    _melleAreaWidth = _melleAttack.areaWidth * squareSize;
    _melleAreaHeight = _melleAttack.areaHeight * squareSize;
}

bool Enemy::checkMelleZone(float px, float py) const
{
    float x = _melleAreaCenterX - _melleAreaWidth * 0.5f;
    float y = _melleAreaCenterY - _melleAreaHeight * 0.5f;
    float w = x + _melleAreaWidth;
    float h = y + _melleAreaHeight;
    return px > x && py > y && px < w && py < h;
}
