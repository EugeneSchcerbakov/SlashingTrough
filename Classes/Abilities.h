//
//  Abilities.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 15/08/15.
//
//

#ifndef __SlashingTrough__Abilities__
#define __SlashingTrough__Abilities__

#include <memory>
#include <vector>

#include "Item.h"

class BackDash : public Ability
{
public:
    static Ptr create(float cooldown, float distance, float duration);
    
public:
    BackDash(float cooldown, float distance, float duration);
    
    void init(Hero *p) override;
    void update(float dt) override;
    void swipeBack() override;
    
protected:
    static const std::string TAG;
    
    float _cooldown;
    float _distance;
    float _duration;
    float _time;
    bool _allow;
};

class BackDashShield : public BackDash
{
public:
    static Ptr create(float cooldown, float distance, float duration, float shieldTime);
    
public:
    BackDashShield(float cooldown, float distance, float duration, float shieldTime);
    
    void update(float dt) override;
    void swipeBack() override;
    
protected:
    void updateBarrier();
    bool isPointUnderBarrier(float x, float y) const;
    
    float _shieldTimer;
    bool _shieldShown;
    
    const float _shieldLiveTime;
};

class Crit : public Ability
{
public:
    static Ptr create(float multiplier, int chance);
    
public:
    Crit(float multiplier, int chance);
    
    virtual void hit(Entity *entity) override;
    
protected:
    float _multiplier;
    int _chance;
};

#endif /* defined(__SlashingTrough__Abilities__) */
