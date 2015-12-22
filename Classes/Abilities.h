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

class Dash : public Ability
{
public:
	static Ptr create(float cooldown, float distance, float duration, bool invulnerable);
    
public:
	Dash(float cooldown, float distance, float duration, bool invulnerable);
    
    void init(Hero *p) override;
    void update(float dt) override;
    void swipeBack() override;
    
protected:
    virtual void makeDash();
    
    static const std::string TAG;
    
    float _cooldown;
    float _distance;
    float _duration;
    float _time;
    bool _allow;

	const bool _invulnerable;
};

class BackDashShield : public Dash
{
public:
	static Ptr create(float cooldown, float distance, float duration, float shieldTime, bool invulnerable);
    
public:
	BackDashShield(float cooldown, float distance, float duration, float shieldTime, bool invulnerable);
    
    void update(float dt) override;
    void swipeBack() override;
    
protected:
    void updateBarrier();
    bool isPointUnderBarrier(float x, float y) const;
    
    float _shieldTimer;
    bool _shieldShown;
    
    const float _shieldLiveTime;
};

class ForwardDash : public Dash
{
public:
	static Ptr create(float cooldown, float distance, float duration, bool invulnerable);
    
public:
	ForwardDash(float cooldown, float distance, float duration, bool invulnerable);
    
    virtual void swipeBack() override;
    virtual void swipeForward() override;
};

class ForwardDashAttack : public ForwardDash
{
public:
	static Ptr create(float cooldown, float distance, float duration, bool invulnerable);
    
public:
	ForwardDashAttack(float cooldown, float distance, float duration, bool invulnerable);
    
protected:
    virtual void makeDash() override;
};

class Vampirism : public Ability
{
public:
    static Ptr create(float healthDrain, float healthReturn);
    
public:
    Vampirism(float healthDrain, float healthReturn);
    
    virtual void update(float dt) override;
    virtual void hit(Entity *entity) override;
    
protected:
    float _localTime;
    
    const float _healthDrain;
    const float _healthReturn;
};

class BurningAura : public Ability
{
public:
    static Ptr create(float damage, float frequency, float radius);
    
public:
    BurningAura(float damage, float frequency, float radius);
    
    virtual void update(float dt) override;
    
protected:
    void performBurning();
    
    float _localTime;
    
    const float _damage;
    const float _frequency;
    const float _radius;
};

class ProjectileAbsorb : public Ability
{
public:
    static Ptr create();
    
public:
    ProjectileAbsorb();
    
    virtual void update(float dt) override;
    virtual void swipeLeft() override;
    virtual void swipeRight() override;
    
protected:
    virtual void updateAbsorber();
    
    bool isPointInArea(float x, float y) const;
    
    float _attackTime;
    float _totalTime;
    
    const float _radius;
};

class ProjectileReflect : public ProjectileAbsorb
{
public:
    static Ptr create();
    
public:
    ProjectileReflect();
    
protected:
    virtual void updateAbsorber() override;
};

class MagicShield : public Ability
{
public:
    static Ptr create(float absorb);
    
public:
    MagicShield(float absorb);
    
    virtual float damage(float receivedDamage);
    
protected:
    const float _absorb;
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

class ExtendedRange : public Ability
{
public:
    static Ptr create(float increase);
    
public:
    ExtendedRange(float increase);
    
    virtual void init(Hero *hero) override;
    
protected:
    float _increase;
};

class Regeneration : public Ability
{
public:
    static Ptr create(float healthPerSecond);
    
public:
    Regeneration(float healthPerSecond);
    
    virtual void update(float dt) override;
    
protected:
    float _localTime;
    
    const float _healthPerSecond;
};

#endif /* defined(__SlashingTrough__Abilities__) */
