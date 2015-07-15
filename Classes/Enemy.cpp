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

Enemy::Enemy(const GameInfo::EnemyType &type, Field *field)
: Entity(Entity::Type::ENEMY)
, Reward(type.killPoints, type.goldPointsReward,
         type.staminaDrainPoints, type.scorePointsReward)
, _field(field)
{
    init(type);
}

void Enemy::idleUpdate(float dt)
{
    if (_goal) {
        // collision
        float goalx = _goal->getPositionX();
        float goaly = _goal->getPositionY();
        float dx = goalx - _x;
        float dy = goaly - _y;
        float len = sqrtf(dx * dx + dy * dy);
        
        if (len < _radius && goaly < _y)
        {
            _goal->addHealth(-_damage);
            if (_goal->isType(Entity::Type::HERO)) {
                Hero *hero = dynamic_cast<Hero *>(_goal);
                hero->addStamina(-_staminaDrainPoints);
            }
            kill();
        }
        
        if (_rangeAttack.allowed) {
            processRangeAttack(dt, len);
        }
    }
}

void Enemy::processRangeAttack(float dt, float len)
{
    if (!_goal->isType(Entity::Type::HERO)) {
        return;
    }
    
    Hero *hero = nullptr;
    hero = dynamic_cast<Hero *>(_goal);
    
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
    }
}

void Enemy::onDamageReceived()
{
    Event event("DamageReceived");
    sendEvent(event);
}

void Enemy::init(const GameInfo::EnemyType &type)
{
    _health = type.health;
    _damage = type.damage;
    _sprite = type.sprite;
    _radius = 150.0f;
    _staminaDrainPoints = type.staminaDrainPoints;
    _rangeAttack = type.rangeAttack;
    _rangeAttackTimer = 0.0f;
}
