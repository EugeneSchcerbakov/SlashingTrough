//
//  Obstacle.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 12/07/15.
//
//

#include "Obstacle.h"
#include "Hero.h"

#include <math.h>

Obstacle::Obstacle(const GameInfo::ObstacleType &type)
: Entity(Entity::Type::OBSTACLE)
, Reward(type.killPoints, type.goldPointsReward,
         type.staminaDrainPoints, type.scorePointsReward)
{
    init(type);
}

void Obstacle::idleUpdate(float dt)
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
                hero->onDamageReceived();
            }
            kill();
        }
    }
}

void Obstacle::addHealth(float health, bool callDamageReceived)
{
    if (health < 0.0f && !_destructible) {
        return;
    } else {
        Entity::addHealth(health);
    }
}

void Obstacle::init(const GameInfo::ObstacleType &type)
{
    _health = type.health;
    _damage = type.damage;
    _sprite = type.sprite;
    _radius = 150.0f;
    _destructible = type.destructible;
    _staminaDrainPoints = type.staminaDrainPoints;
}
