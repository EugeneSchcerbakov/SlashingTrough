//
//  Projectile.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 14/07/15.
//
//

#include "Projectile.h"
#include "Hero.h"

#include <math.h>

Projectile::Projectile(const GameInfo::ProjectileType &projectile, float x, float y, float dx, float dy)
: Entity(Entity::Type::PROJECTILE)
, _dx(dx)
, _dy(dy)
, _speed(projectile.speed)
, _lifeTime(projectile.lifeTime)
, _healthDmg(projectile.healthDamage)
, _staminaDmg(projectile.staminaDamage)
, _localTime(0.0f)
{
    _x = x;
    _y = y;
    _health = 1.0f;
    _radius = 50.0f;
    _sprite = projectile.texture;
}

void Projectile::idleUpdate(float dt)
{
    if (_goal && _goal->isType(Entity::Type::HERO)) {
        _x += _dx * _speed * dt;
        _y += _dy * _speed * dt;
        
        float dx = _goal->getPositionX() - _x;
        float dy = _goal->getPositionY() - _y;
        
        float len = sqrtf(dx * dx + dy * dy);
        
        if (len < 80.0f) {
            _goal->addHealth(-_healthDmg);
            if (_goal->isType(Entity::Type::HERO)) {
                auto hero = dynamic_cast<Hero *>(_goal);
                hero->addStamina(-_staminaDmg);
                hero->onDamageReceived();
            }
            kill();
        }
        
        _localTime += dt;
        if (_localTime > _lifeTime) {
            _localTime = 0.0;
            kill();
        }
    }
}

void Projectile::addHealth(float health, bool callDamageReceived)
{
    // just nothing
}
