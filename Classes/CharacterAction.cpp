//
//  CharacterAction.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 09/04/15.
//
//

#include "CharacterAction.h"

CharacterAction::CharacterAction(Type type, float duration, float deltaX, float deltaY)
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

CharacterAction::CharacterAction()
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

bool CharacterAction::IsType(Type type) const
{
    return _type == type;
}

float CharacterAction::GetFinishX() const
{
    return _finishPosX;
}

float CharacterAction::GetFinishY() const
{
    return _finishPosY;
}

float CharacterAction::GetDeltaX() const
{
    return _deltaX;
}

float CharacterAction::GetDeltaY() const
{
    return _deltaY;
}

float CharacterAction::GetDuration() const
{
    return _duration;
}

bool CharacterAction::IsReady() const
{
    return !_isFinished && !_isProcessing;
}

void CharacterAction::Start()
{
    _isFinished = false;
    _isProcessing = true;
}

void CharacterAction::Finish()
{
    _isFinished = true;
    _isProcessing = false;
}

void CharacterAction::SetFinishPosX(float x)
{
    _finishPosX = x;
}

void CharacterAction::SetFinishPosY(float y)
{
    _finishPosY = y;
}
