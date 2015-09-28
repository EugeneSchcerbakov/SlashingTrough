//
//  Abilities.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 15/08/15.
//
//

#include "Abilities.h"
#include "Hero.h"
#include "Enemy.h"
#include "Utils.h"
#include "Log.h"

// Backsliding

const std::string BackDash::TAG = "BackslidngAction";

BackDash::BackDash(float cooldown, float distance, float duration)
: Ability()
, _cooldown(cooldown)
, _distance(distance)
, _duration(duration)
, _time(0.0)
, _allow(true)
{
}

BackDash::Ptr BackDash::create(float cooldown, float distance, float duration)
{
    return std::make_shared<BackDash>(cooldown, distance, duration);
}

void BackDash::init(Hero *p)
{
    Ability::init(p);
}

void BackDash::update(float dt)
{
    Ability::update(dt);
    
    if (!_allow && !_hero->isActionInQueue(TAG)) {
        _time += dt;
        if (_time >= _cooldown) {
            _allow = true;
            _time = 0.0f;
        }
    }
}

void BackDash::swipeBack()
{
    Ability::swipeBack();
    
    if (_allow)
    {
        HeroAction *action = new JumpBack(_hero, _duration, -_distance);
        if (_hero->isAbleToPerform(action))
        {
            Event e("JumpBackStart");
            e.variables.setFloat("duration", _duration);
            action->setEvent(e);
            action->setTag(TAG);
        
            _hero->addAction(action);
            _allow = false;
        }
    }
}

// BackslidingShield

BackDashShield::Ptr BackDashShield::create(float cooldown, float distance, float duration, float shieldTime)
{
    return std::make_shared<BackDashShield>(cooldown, distance, duration, shieldTime);
}

BackDashShield::BackDashShield(float cooldown, float distance, float duration, float shieldTime)
: BackDash(cooldown, distance, duration)
, _shieldLiveTime(shieldTime)
, _shieldShown(false)
{
}

void BackDashShield::update(float dt)
{
    Ability::update(dt);
    
    if (_shieldShown) {
        if (_shieldTimer > 0.0f) {
            _shieldTimer -= dt;
            updateBarrier();
        } else {
            Event e("HideShield");
            _hero->sendEvent(e);
            _shieldShown = false;
        }
    }
    
    if (!_allow && !_hero->isActionInQueue(TAG)) {
        if (_time >= _cooldown) {
            _allow = true;
            _time = 0.0f;
            Event e("JumpBackEnd");
            _hero->sendEvent(e);
        } else {
            _time += dt;
        }
    }
}

void BackDashShield::swipeBack()
{
    Ability::swipeBack();
    
    if (_allow)
    {
        HeroAction *action = new JumpBack(_hero, _duration, -_distance);
        if (_hero->isAbleToPerform(action))
        {
            Event e("JumpBackStart");
            e.variables.setFloat("duration", _duration);
            e.variables.setBool("showShield", true);
            action->setEvent(e);
            action->setTag(TAG);
            
            _hero->addAction(action);
            _shieldTimer = _shieldLiveTime;
            _allow = false;
            _shieldShown = true;
        }
    }
}

void BackDashShield::updateBarrier()
{
    if (_hero->getGoals()) {
        const Entities& entities = *_hero->getGoals();
        for (auto it = entities.begin(); it != entities.end(); ++it) {
            Entity *entity = (*it);
            if (entity) {
                if (entity->isType(Entity::Type::PROJECTILE)) {
                    float x = entity->getPositionX();
                    float y = entity->getPositionY();
                    if (isPointUnderBarrier(x, y)) {
                        entity->kill();
                        _hero->sendEvent(Event("ShieldDamageReceived"));
                    }
                } else if (entity->isType(Entity::Type::ENEMY)) {
                    Enemy *enemy = dynamic_cast<Enemy *>(entity);
                    if (enemy && enemy->isState(Enemy::State::MELLE_ATTACK_BEGIN)) {
                        float x = enemy->getMelleAreaCenterX();
                        float y = enemy->getMelleAreaCenterY();
                        if (isPointUnderBarrier(x, y)) {
                            enemy->discardMelleAttack();
                            _hero->sendEvent(Event("ShieldDamageReceived"));
                        }
                    }
                }
            }
        }
    }
}

bool BackDashShield::isPointUnderBarrier(float x, float y) const
{
    const float barrierRadius = 200.0f;
    
    float xface = 0.0f;
    float yface = 1.0f;
    
    float x1 = _hero->getPositionX();
    float y1 = _hero->getPositionY();
    
    float dx = x - x1;
    float dy = y - y1;
    
    float len = sqrtf(dx * dx + dy * dy);
    
    float n = len;
    n = 1.0f / n;
    float nx = dx * n;
    float ny = dy * n;
    
    float dotp = nx * xface + ny * yface;
    
    return len <= barrierRadius && dotp >= 0.5f;
}

Ability::Ptr Vampirism::create(float healthDrain, float healthReturn)
{
    return std::make_shared<Vampirism>(healthDrain, healthReturn);
}

Vampirism::Vampirism(float healthDrain, float healthReturn)
: Ability()
, _healthDrain(healthDrain)
, _healthReturn(healthReturn / 100.0f)
{
    _localTime = healthDrain;
}

void Vampirism::update(float dt)
{
    if (_localTime <= 0.0f) {
        _hero->addHealth(-_healthDrain, false);
        _localTime = 1.0f;
    } else {
        _localTime -= dt;
    }
}

void Vampirism::hit(Entity *entity)
{
    if (entity && entity->isType(Entity::Type::ENEMY))
    {
        float damaged = _hero->getDamage();
        float returns = ceilf(damaged * _healthReturn);
        returns = std::max(1.0f, returns);
        
        _hero->addHealth(returns);
    }
}

Ability::Ptr BurningAura::create(float damage, float frequency, float radius)
{
    return std::make_shared<BurningAura>(damage, frequency, radius);
}

BurningAura::BurningAura(float damage, float frequency, float radius)
: Ability()
, _damage(damage)
, _frequency(frequency)
, _radius(radius)
, _localTime(0.0f)
{
}

void BurningAura::update(float dt)
{
    if (_localTime < _frequency) {
        _localTime += dt;
    } else {
        performBurning();
        _localTime = 0.0f;
    }
}

void BurningAura::performBurning()
{
    if (_hero->getGoals())
    {
        Entities& entities = *_hero->getGoals();
        for (auto it = entities.begin(); it != entities.end(); ++it)
        {
            Entity *entity = (*it);
            
            if (entity && entity->isAlive() && entity->isType(Entity::Type::ENEMY))
            {
                float x1 = _hero->getPositionX();
                float y1 = _hero->getPositionY();
                
                float x2 = entity->getPositionX();
                float y2 = entity->getPositionY();
                
                float dx = x2 - x1;
                float dy = y2 - y1;
                
                float len = sqrtf(dx * dx + dy * dy);
                
                if (len <= _radius)
                {
                    entity->addHealth(-_damage);
                }
            }
        }
    }
}

Ability::Ptr MagicShield::create(float absorb)
{
    return std::make_shared<MagicShield>(absorb);
}

MagicShield::MagicShield(float absorb)
: Ability()
, _absorb(absorb)
{
}

float MagicShield::damage(float receivedDamage)
{
    float actual = math::clamp(receivedDamage + _absorb, receivedDamage, 0.0f);
    return actual;
}

Ability::Ptr Crit::create(float multiplier, int chance)
{
    return std::make_shared<Crit>(multiplier, chance);
}

Crit::Crit(float multiplier, int chance)
: Ability()
, _multiplier(multiplier)
, _chance(chance)
{
}

void Crit::hit(Entity *entity)
{
    if (entity && entity->isType(Entity::Type::ENEMY))
    {
        bool passed = misc::random(0, 100) <= _chance;
        if (passed)
        {
            float damage = _hero->getDamage();
            float critic = std::max(0.0f, damage * _multiplier);
            float result = std::max(0.0f, critic - damage);
            entity->addHealth(-result);
        }
    }
}

Ability::Ptr ExtendedRange::create(float increase)
{
    return std::make_shared<ExtendedRange>(increase);
}

ExtendedRange::ExtendedRange(float increase)
: Ability()
, _increase(increase)
{
}

void ExtendedRange::init(Hero *hero)
{
    Ability::init(hero);
    hero->extendRadius(_increase);
}

Ability::Ptr Regeneration::create(float healthPerSecond)
{
    return std::make_shared<Regeneration>(healthPerSecond);
}

Regeneration::Regeneration(float healthPerSecond)
: Ability()
, _healthPerSecond(healthPerSecond)
, _localTime(0.0f)
{
}

void Regeneration::update(float dt)
{
    if (_localTime < 1.0f) {
        _localTime += dt;
    } else {
        _hero->addHealth(_healthPerSecond);
        _localTime = 0.0f;
    }
}
