//
//  Projectile.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 14/07/15.
//
//

#ifndef __SlashingTrough__Projectile__
#define __SlashingTrough__Projectile__

#include <stdio.h>

#include "ModelBase.h"
#include "GameInfo.h"

class Projectile : public Entity
{
public:
    Projectile(const GameInfo::ProjectileType &projectile, float x, float y, float dx, float dy);
    
    void idleUpdate(float dt) override;
    void addHealth(float health, bool callDamageReceived = true) override;
    
    void setSpeed(float speed);
    void setDirection(float dx, float dy);
    void setReflected(bool reflected);
    
    float getSpeed() const;
    float getDirX() const;
    float getDirY() const;
    
    bool isReflected() const;
    
private:
    void tryDamage(Entity *goal);
    
    float _dx, _dy;
    float _speed;
    float _localTime;
    
    bool _reflected;
    
    const float _lifeTime;
    const float _healthDmg, _staminaDmg;
};

#endif /* defined(__SlashingTrough__Projectile__) */
