//
//  HeroAction.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 11/07/15.
//
//

#include "HeroAction.h"
#include "Hero.h"
#include "Utils.h"

HeroAction::HeroAction(Hero *hero, float duration, Type type)
: _hero(hero)
, _duration(duration)
, _type(type)
, _localTime(0.0f)
, _isStarted(false)
, _isAttacked(false)
, _isFinished(false)
{
}

HeroAction::~HeroAction()
{
}

void HeroAction::start()
{
    _isStarted = true;
}

void HeroAction::update(float dt)
{
    if (!_isFinished) {
        float ts = 1.0f / _duration;
        _localTime += dt * ts;
        if (_localTime >= 1.0f) {
            _isFinished = true;
            _localTime = 1.0f;
        }
    }
}

void HeroAction::setEvent(const Event &event)
{
    _event = event;
}

bool HeroAction::isFinished() const
{
    return _isFinished;
}

bool HeroAction::isStarted() const
{
    return _isStarted;
}

bool HeroAction::isType(HeroAction::Type type) const
{
    return _type == type;
}

bool HeroAction::isTag(const std::string &tag) const
{
    return _tag == tag;
}

const Event& HeroAction::getEvent() const
{
    return _event;
}

void HeroAction::setTag(const std::string &tag)
{
    _tag = tag;
}

// HeroActionMove

AttackAndMove::AttackAndMove(Hero *hero, float duration, float deltaX, float deltaY)
: HeroAction(hero, duration, Type::ATTACK_MOVE)
{
    HeroAction *lastAction = hero->getLastAction();
    if (lastAction && lastAction->isType(Type::ATTACK_MOVE)) {
        AttackAndMove *move = dynamic_cast<AttackAndMove *>(lastAction);
        _startX = move->_finishX;
        _startY = move->_finishY;
    } else {
        _startX = _hero->getPositionX();
        _startY = _hero->getPositionY();
    }
    
    _finishX = _startX + deltaX;
    _finishY = _startY + deltaY;
}

void AttackAndMove::update(float dt)
{
    HeroAction::update(dt);
    
    if (!_isAttacked) {
        float wait = 0.25f;
        if (_localTime >= wait) {
            _hero->attack();
            _isAttacked = true;
        }
    }
    
    float t = 0.0f;
    t = -0.5f * (cosf(math::PI * _localTime) - 1.0f); // tween sine in out
    t = math::clamp(t, 0.0f, 1.0f);

    if (_finishX - _startX != 0.0f) {
        float x = math::lerp(_startX, _finishX, t);
        _hero->setPositionX(x);
    }
    if (_finishY - _startY != 0.0f) {
        float y = math::lerp(_startY, _finishY, t);
        _hero->setPositionY(y);
    }
}

float AttackAndMove::getFinishX() const
{
    return _finishX;
}

float AttackAndMove::getFinishY() const
{
    return _finishY;
}

// HeroActionJumpBack

JumpBack::JumpBack(Hero *hero, float duration, float jumpDist)
: HeroAction(hero, duration, Type::JUMPBACK)
{
    _startY = hero->getPositionY();
    
    HeroAction *lastAction = hero->getLastAction();
    if (lastAction && lastAction->isType(Type::JUMPBACK)) {
        auto jumpback = dynamic_cast<JumpBack *>(lastAction);
        _startY = jumpback->getFinishY();
    }

    _endY = _startY + jumpDist;
}

void JumpBack::start()
{
    HeroAction::start();
    _hero->setRunning(false);
}

void JumpBack::update(float dt)
{
    HeroAction::update(dt);
    
    float t = 0.0f;
    t = -0.5f * (cosf(math::PI * _localTime) - 1.0f); // tween sine in out
    t = math::clamp(t, 0.0f, 1.0f);
    
    float y = math::lerp(_startY, _endY, t);
    
    _hero->setPositionY(y);
    
    if (_isFinished) {
        _hero->setRunning(true);
    }
}

float JumpBack::getFinishY() const
{
    return _endY;
}
