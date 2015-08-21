//
//  EquipFeature.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 15/08/15.
//
//

#include "EquipFeature.h"
#include "Hero.h"
#include "Enemy.h"

EquipFeature::EquipFeature()
: _owner(nullptr)
{
}

EquipFeature::~EquipFeature()
{
}

void EquipFeature::init(Hero *owner)
{
    _owner = owner;
}

void EquipFeature::update(float dt)
{
}

void EquipFeature::onSwipeBack()
{
}

void EquipFeature::onSwipeRight()
{
}

void EquipFeature::onSwipeLeft()
{
}

// Backsliding

const std::string Backsliding::TAG = "BackslidngAction";

Backsliding::Backsliding(float cooldown, float distance, float duration)
: EquipFeature()
, _cooldown(cooldown)
, _distance(distance)
, _duration(duration)
, _time(0.0)
, _allow(true)
{
}

Backsliding::Ptr Backsliding::create(float cooldown, float distance, float duration)
{
    return std::make_shared<Backsliding>(cooldown, distance, duration);
}

void Backsliding::init(Hero *owner)
{
    EquipFeature::init(owner);
}

void Backsliding::update(float dt)
{
    EquipFeature::update(dt);
    
    if (!_allow && !_owner->isActionInQueue(TAG)) {
        _time += dt;
        if (_time >= _cooldown) {
            _allow = true;
            _time = 0.0f;
        }
    }
}

void Backsliding::onSwipeBack()
{
    EquipFeature::onSwipeBack();
    
    if (_allow)
    {
        HeroAction *action = new JumpBack(_owner, _duration, -_distance);
        if (_owner->isAbleToPerform(action))
        {
            Event e("BackslideStart");
            e.variables.setFloat("duration", _duration);
            action->setEvent(e);
            action->setTag(TAG);
        
            _owner->addAction(action);
            _allow = false;
        }
    }
}

// BackslidingShield

BackslidingShield::Ptr BackslidingShield::create(float cooldown, float distance, float duration, float shieldTime)
{
    return std::make_shared<BackslidingShield>(cooldown, distance, duration, shieldTime);
}

BackslidingShield::BackslidingShield(float cooldown, float distance, float duration, float shieldTime)
: Backsliding(cooldown, distance, duration)
, _shieldLiveTime(shieldTime)
, _shieldShown(false)
{
}

void BackslidingShield::update(float dt)
{
    EquipFeature::update(dt);
    
    if (_shieldShown) {
        if (_shieldTimer > 0.0f) {
            _shieldTimer -= dt;
            updateBarrier();
        } else {
            Event e("HideShield");
            _owner->sendEvent(e);
            _shieldShown = false;
        }
    }
    
    if (!_allow && !_owner->isActionInQueue(TAG)) {
        if (_time >= _cooldown) {
            _allow = true;
            _time = 0.0f;
            Event e("JumpBackEnd");
            _owner->sendEvent(e);
        } else {
            _time += dt;
        }
    }
}

void BackslidingShield::onSwipeBack()
{
    EquipFeature::onSwipeBack();
    
    if (_allow)
    {
        HeroAction *action = new JumpBack(_owner, _duration, -_distance);
        if (_owner->isAbleToPerform(action))
        {
            Event e("JumpBackStart");
            e.variables.setFloat("duration", _duration);
            e.variables.setBool("showShield", true);
            action->setEvent(e);
            action->setTag(TAG);
            
            _owner->addAction(action);
            _shieldTimer = _shieldLiveTime;
            _allow = false;
            _shieldShown = true;
        }
    }
}

void BackslidingShield::updateBarrier()
{
    if (_owner->getGoals()) {
        const Entities& entities = *_owner->getGoals();
        for (auto it = entities.begin(); it != entities.end(); ++it) {
            Entity *entity = (*it);
            if (entity) {
                if (entity->isType(Entity::Type::PROJECTILE)) {
                    float x = entity->getPositionX();
                    float y = entity->getPositionY();
                    if (isPointUnderBarrier(x, y)) {
                        entity->kill();
                        _owner->sendEvent(Event("ShieldDamageReceived"));
                    }
                } else if (entity->isType(Entity::Type::ENEMY)) {
                    Enemy *enemy = dynamic_cast<Enemy *>(entity);
                    if (enemy && enemy->isState(Enemy::State::MELLE_ATTACK_BEGIN)) {
                        float x = enemy->getMelleAreaCenterX();
                        float y = enemy->getMelleAreaCenterY();
                        if (isPointUnderBarrier(x, y)) {
                            enemy->discardMelleAttack();
                            _owner->sendEvent(Event("ShieldDamageReceived"));
                        }
                    }
                }
            }
        }
    }
}

bool BackslidingShield::isPointUnderBarrier(float x, float y) const
{
    const float barrierRadius = 200.0f;
    
    float xface = 0.0f;
    float yface = 1.0f;
    
    float x1 = _owner->getPositionX();
    float y1 = _owner->getPositionY();
    
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
