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
#include "Projectile.h"
#include "Utils.h"
#include "Log.h"

// Backsliding

const std::string Dash::TAG = "Dash";

Dash::Dash(float cooldown, float distance, float duration, bool invulnerable)
: Ability()
, _cooldown(cooldown)
, _distance(distance)
, _duration(duration)
, _time(0.0)
, _invulnerable(invulnerable)
, _allow(true)
{
}

Ability::Ptr Dash::create(float cooldown, float distance, float duration, bool invulnerable)
{
	return std::make_shared<Dash>(cooldown, distance, duration, invulnerable);
}

void Dash::init(Hero *p)
{
    Ability::init(p);
}

void Dash::update(float dt)
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

void Dash::swipeBack()
{
    Ability::swipeBack();
    
    makeDash();
}

void Dash::makeDash()
{
    if (_allow)
    {
        HeroAction::Ptr action = JumpBack::create(_hero, _duration, _distance, _invulnerable);
        if (_hero->isAbleToPerform(action))
        {
            Event e("JumpBackStart");
            e.variables.setFloat("duration", _duration);
            e.variables.setBool("showDistortion", _distance > 0.0f);
            action->setEvent(e);
            action->setTag(TAG);
            
            _hero->addAction(action);
            _allow = false;
        }
    }
}

// BackslidingShield

Ability::Ptr BackDashShield::create(float cooldown, float distance, float duration, float shieldTime)
{
    return std::make_shared<BackDashShield>(cooldown, distance, duration, shieldTime);
}

BackDashShield::BackDashShield(float cooldown, float distance, float duration, float shieldTime)
: Dash(cooldown, distance, duration, false)
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
		HeroAction::Ptr action = JumpBack::create(_hero, _duration, _distance, _invulnerable);
        if (_hero->isAbleToPerform(action))
        {
            auto func = [this]()
            {
                _shieldTimer = _shieldLiveTime;
                _shieldShown = true;
            };
            
            Event e("JumpBackStart");
            e.variables.setFloat("duration", _duration);
            e.variables.setBool("showShield", true);
            action->setEvent(e);
            action->setCallback(func);
            action->setTag(TAG);
            
            _hero->addAction(action);
            
            _allow = false;
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

// BackDashShot

BackDashShot::Ptr BackDashShot::create(float cooldown, float distance, float duration, int shotsAmount,
                  float shotInterval, float damageModifier, bool nearestTarget)
{
    return std::make_shared<BackDashShot>(cooldown, distance, duration, shotsAmount, shotInterval, damageModifier, nearestTarget);
}

BackDashShot::BackDashShot(float cooldown, float distance, float duration, int shotsAmount,
             float shotInterval, float damageModifier, bool nearestTarget)
: Dash(cooldown, distance, duration, false)
, _shotsAmount(shotsAmount)
, _shotInterval(shotInterval)
, _damageModifier(damageModifier)
, _nearestTraget(nearestTarget)
, _shotTimer(0.0f)
, _shotsToMake(0)
{
}

void BackDashShot::update(float dt)
{
    Dash::update(dt);
    
    if (_shotsToMake > 0)
    {
        if (_shotTimer <= 0.0f)
        {
            if (_nearestTraget) {
                makeAimedShot();
            } else {
                makeForwardShot();
            }
            
            _shotTimer = _shotInterval;
            _shotsToMake -= 1;
        }
        else
        {
            _shotTimer -= dt;
        }
    }
}

void BackDashShot::swipeBack()
{
    Ability::swipeBack();
    
    if (_allow)
    {
        HeroAction::Ptr action = JumpBack::create(_hero, _duration, _distance, _invulnerable);
        if (_hero->isAbleToPerform(action))
        {
            auto func = [this]()
            {
                _shotsToMake = _shotsAmount;
                _shotTimer = 0.0f;
            };
            
            Event e("JumpBackStart");
            e.variables.setFloat("duration", _duration);
            action->setEvent(e);
            action->setCallback(func);
            action->setTag(TAG);
            
            _hero->addAction(action);
            
            _allow = false;
        }
    }
}

void BackDashShot::makeForwardShot()
{
    if (!_hero) {
        return;
    }
    
    ItemWeapon *weapon = _hero->getWeapon();
    if (!weapon) {
        WRITE_WARN("Invalid weapon");
        return;
    }
    
    float shotdamage = weapon->getDamage() * _damageModifier;
    
    float yoffset = 20.0f;
    float pos_x = _hero->getPositionX();
    float pos_y = _hero->getPositionY() + yoffset;
    float dir_x = 0.0f;
    float dir_y = 1.0f;
    
    GameInfo::ProjectileType desc;
    desc.healthDamage = shotdamage;
    desc.lifeTime = 2.0f;
    desc.speed = 1200.0f;
    desc.texture = "gamefield/projectile_0.png";
    
    Projectile *projectile = new Projectile(desc, pos_x, pos_y, dir_x, dir_y);
    projectile->setReflected(true);
    projectile->setGoal(_hero);
    
    _hero->shootProjectile(projectile);
}

void BackDashShot::makeAimedShot()
{
    if (!_hero) {
        return;
    }
    
    ItemWeapon *weapon = _hero->getWeapon();
    if (!weapon) {
        WRITE_WARN("Invalid weapon");
        return;
    }
    
    // determin nearest enemy
    
    Enemy *nearest = nullptr;
    Entities *entities = _hero->getGoals();
    
    if (!entities) {
        return;
    }
    
    float minLen = std::numeric_limits<float>::max();
    
    for (auto it = entities->begin(); it != entities->end(); it++)
    {
        Enemy *enemy = dynamic_cast<Enemy *>(*it);
        
        if (!enemy) {
            continue;
        }
        
        float x1 = _hero->getPositionX();
        float y1 = _hero->getPositionY();
        
        float x2 = enemy->getPositionX();
        float y2 = enemy->getPositionY();
        
        // dont shoot to enemies behind the hero
        if (y2 < y1) {
            continue;
        }
        
        float dx = x2 - x1;
        float dy = y2 - y1;
        
        float len = sqrtf(dx * dx + dy * dy);
        
        if (len < minLen) {
            minLen = len;
            nearest = enemy;
        }
    }
    
    if (nearest)
    {
        float yoffset = 0.0f;
        float px = _hero->getPositionX();
        float py = _hero->getPositionY() + yoffset;
        float dx = nearest->getPositionX() - px;
        float dy = nearest->getPositionY() - py;
        float plen = sqrtf(dx * dx + dy * dy);
        
        float n = plen;
        n = 1.0f / n;
        float nx = dx * n;
        float ny = dy * n;
        
        float shotdamage = weapon->getDamage() * _damageModifier;
        
        GameInfo::ProjectileType desc;
        desc.healthDamage = shotdamage;
        desc.lifeTime = 1.0f;
        desc.speed = 1000.0f;
        desc.texture = "gamefield/projectile_0.png";
        
        Projectile *projectile = new Projectile(desc, px, py, nx, ny);
        projectile->setReflected(true);
        projectile->setGoal(_hero);
        
        _hero->shootProjectile(projectile);
    }
}

// ForwardDash

Ability::Ptr ForwardDash::create(float cooldown, float distance, float duration, bool invulnerable)
{
	return std::make_shared<ForwardDash>(cooldown, distance, duration, invulnerable);
}

ForwardDash::ForwardDash(float cooldown, float distance, float duration, bool invulnerable)
: Dash(cooldown, distance, duration, invulnerable)
{
}

void ForwardDash::swipeBack()
{
    Ability::swipeBack();
}

void ForwardDash::swipeForward()
{
    makeDash();
}

Ability::Ptr ForwardDashAttack::create(float cooldown, float distance, float duration, bool invulnerable)
{
	return std::make_shared<ForwardDashAttack>(cooldown, distance, duration, invulnerable);
}

ForwardDashAttack::ForwardDashAttack(float cooldown, float distance, float duration, bool invulnerable)
: ForwardDash(cooldown, distance, duration, invulnerable)
{
}

void ForwardDashAttack::makeDash()
{
    if (_allow)
    {
		HeroAction::Ptr action = JumpForwardAttack::create(_hero, _duration, _distance, _invulnerable);
        if (_hero->isAbleToPerform(action))
        {
            Event e("JumpForwardAttack");
            e.variables.setFloat("duration", _duration);
            action->setEvent(e);
            action->setTag(TAG);
            
            _hero->addAction(action);
            _allow = false;
        }
    }
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

Ability::Ptr ProjectileAbsorb::create()
{
    return std::make_shared<ProjectileAbsorb>();
}

ProjectileAbsorb::ProjectileAbsorb()
: Ability()
, _attackTime(0.0f)
, _totalTime(0.0f)
, _radius(250.0f)
{
}

void ProjectileAbsorb::update(float dt)
{
    if (_attackTime > 0.0f) {
        _attackTime -= dt;
        
        float lower = math::lerp(0.0f, _totalTime, 0.2f);
        float upper = math::lerp(0.0f, _totalTime, 0.8f);
        
        if (_attackTime >= lower && _attackTime <= upper)
        {
            updateAbsorber();
        }
    }
}

void ProjectileAbsorb::swipeLeft()
{
    _totalTime = _hero->getWeapon()->getSpeed();
    _attackTime = _totalTime;
}

void ProjectileAbsorb::swipeRight()
{
    _totalTime = _hero->getWeapon()->getSpeed();
    _attackTime = _totalTime;
}

void ProjectileAbsorb::updateAbsorber()
{
    Entities& entities = *_hero->getGoals();
    for (auto it = entities.begin(); it != entities.end(); ++it)
    {
        Entity *entity = (*it);
        
        if (entity && entity->isAlive() && entity->isType(Entity::Type::PROJECTILE))
        {
            float x = entity->getPositionX();
            float y = entity->getPositionY();
            
            if (isPointInArea(x, y))
            {
                entity->kill();
            }
        }
    }
}

bool ProjectileAbsorb::isPointInArea(float x, float y) const
{
    float x1 = _hero->getPositionX();
    float y1 = _hero->getPositionY();
    
    float dx = x - x1;
    float dy = y - y1;
    
    float len = sqrtf(dx * dx + dy * dy);
    
    return len <= _radius;
}

Ability::Ptr ProjectileReflect::create()
{
    return std::make_shared<ProjectileReflect>();
}

ProjectileReflect::ProjectileReflect()
: ProjectileAbsorb()
{
}

void ProjectileReflect::updateAbsorber()
{
    Entities& entities = *_hero->getGoals();
    for (auto it = entities.begin(); it != entities.end(); ++it)
    {
        Entity *entity = (*it);
        
        if (entity && entity->isAlive() && entity->isType(Entity::Type::PROJECTILE))
        {
            Projectile *projectile = dynamic_cast<Projectile *>(entity);
            
            if (projectile->isReflected()) {
                continue;
            }
            
            float x1 = _hero->getPositionX();
            float y1 = _hero->getPositionY();
            
            float x2 = projectile->getPositionX();
            float y2 = projectile->getPositionY();
            
            float dx = x2 - x1;
            float dy = y2 - y1;
            
            float len = sqrtf(dx * dx + dy * dy);
            
            float n = len;
            n = 1.0f / n;
            float nx = dx * n;
            float ny = dy * n;
            
            float xface = 0.0f;
            float yface = 1.0f;
            float dotf = nx * xface + ny * yface;
            
            if (len <= _radius && dotf >= 0.9f)
            {
                float dirx = projectile->getDirX();
                float diry = projectile->getDirY();
                
                float dotp = nx * dirx + ny * diry;
                
                float rx = dirx - 2.0f * nx * dotp;
                float ry = diry - 2.0f * ny * dotp;
                
                projectile->setDirection(rx, ry);
                projectile->setReflected(true);
                projectile->setSpeed(projectile->getSpeed() * 1.5f);
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
