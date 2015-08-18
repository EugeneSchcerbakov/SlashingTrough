//
//  EquipFeature.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 15/08/15.
//
//

#include "EquipFeature.h"
#include "Hero.h"

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
            Event e("JumpBack");
            e.variables.setFloat("duration", _duration);
            action->setEvent(e);
            action->setTag(TAG);
        
            _owner->addAction(action);
            _allow = false;
        }
    }
}
