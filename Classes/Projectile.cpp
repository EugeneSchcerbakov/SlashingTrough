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
, _localTime(0.0f)
, _reflected(false)
{
    _x = x;
    _y = y;
    _health = 1.0f;
    _radius = 50.0f;
    _sprite = projectile.texture;
}

void Projectile::idleUpdate(float dt)
{
    _x += _dx * _speed * dt;
    _y += _dy * _speed * dt;
    
    _localTime += dt;
    if (_localTime > _lifeTime) {
        _localTime = 0.0;
        kill();
    }
    
    if (_goal)
    {
        if (_reflected && _goal->isType(Entity::Type::HERO))
        {
            Hero *hero = dynamic_cast<Hero *>(_goal);
            Entities& entities = *hero->getGoals();
            
            for (auto it = entities.begin(); it != entities.end(); ++it)
            {
                Entity *entity = (*it);
                
                if (entity && entity->isAlive() && !entity->isType(Entity::Type::PROJECTILE))
                {
                    tryDamage(entity);
                }
            }
        }
        else
        {
            tryDamage(_goal);
        }
    }
}

void Projectile::addHealth(float health, bool callDamageReceived)
{
    // just nothing
}

void Projectile::tryDamage(Entity *goal)
{
    if (!goal) {
        return;
    }
    
    float dx = goal->getPositionX() - _x;
    float dy = goal->getPositionY() - _y;
    
    float len = sqrtf(dx * dx + dy * dy);
    
    if (len < 80.0f)
    {
        Event e("HitedByProjectile");
        e.variables.setFloat("x", _x);
        e.variables.setFloat("y", _y);
        
        goal->addHealth(-_healthDmg);
		goal->sendEvent(e);
        kill();
    }
}

void Projectile::setSpeed(float speed)
{
    _speed = speed;
}

void Projectile::setDirection(float dx, float dy)
{
    _dx = dx;
    _dy = dy;
}

void Projectile::setReflected(bool reflected)
{
    _reflected = reflected;
}

float Projectile::getSpeed() const
{
    return _speed;
}

float Projectile::getDirX() const
{
    return _dx;
}

float Projectile::getDirY() const
{
    return _dy;
}

bool Projectile::isReflected() const
{
    return _reflected;
}
