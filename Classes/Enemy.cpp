//
//  Enemy.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 12/07/15.
//
//

#include "Enemy.h"
#include "Hero.h"

Enemy::Enemy(const GameInfo::EnemyType &type)
: Entity(Entity::Type::ENEMY)
, Reward(type.killPoints, type.goldPointsReward,
         type.staminaDrainPoints, type.scorePointsReward)
{
    init(type);
}

void Enemy::idleUpdate(float dt)
{
    if (_goal) {
        float goalx = _goal->getPositionX();
        float goaly = _goal->getPositionY();
        float dx = _x - goalx;
        float dy = _y - goaly;
        float len = sqrtf(dx * dx + dy * dy);
        if (len < _radius && goaly < _y) {
            _goal->addHealth(-_damage);
            if (_goal->isType(Entity::Type::HERO)) {
                Hero *hero = dynamic_cast<Hero *>(_goal);
                hero->addStamina(-_staminaDrainPoints);
            }
            kill();
        }
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
}
