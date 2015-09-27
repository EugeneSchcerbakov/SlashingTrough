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
    
private:
    const float _dx, _dy;
    const float _speed;
    const float _lifeTime;
    const float _healthDmg, _staminaDmg;
    
    float _localTime;
};

#endif /* defined(__SlashingTrough__Projectile__) */
