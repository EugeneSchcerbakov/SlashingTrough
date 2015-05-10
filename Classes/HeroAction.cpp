//
//  HeroAction.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 09/04/15.
//
//

#include "HeroAction.h"

HeroAction::HeroAction(Type type, float duration, float deltaX, float deltaY)
: _type(type)
, _duration(duration)
, _deltaX(deltaX)
, _deltaY(deltaY)
, _finishPosX(0.0f)
, _finishPosY(0.0f)
, _isProcessing(false)
, _isFinished(false)
{
}

HeroAction::HeroAction()
: _type(Type::NONE)
, _duration(0.0f)
, _deltaX(0.0f)
, _deltaY(0.0f)
, _finishPosX(0.0f)
, _finishPosY(0.0f)
, _isProcessing(false)
, _isFinished(false)
{
}

bool HeroAction::IsType(Type type) const
{
    return _type == type;
}

float HeroAction::GetFinishX() const
{
    return _finishPosX;
}

float HeroAction::GetFinishY() const
{
    return _finishPosY;
}

float HeroAction::GetDeltaX() const
{
    return _deltaX;
}

float HeroAction::GetDeltaY() const
{
    return _deltaY;
}

float HeroAction::GetDuration() const
{
    return _duration;
}

bool HeroAction::IsReady() const
{
    return !_isFinished && !_isProcessing;
}

void HeroAction::Start()
{
    _isFinished = false;
    _isProcessing = true;
}

void HeroAction::Finish()
{
    _isFinished = true;
    _isProcessing = false;
}

void HeroAction::SetFinishPosX(float x)
{
    _finishPosX = x;
}

void HeroAction::SetFinishPosY(float y)
{
    _finishPosY = y;
}
