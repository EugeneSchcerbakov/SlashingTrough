//
//  Character.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 06/04/15.
//
//

#include "Character.h"

#include "GameInfo.h"

Character::Ptr Character::Create()
{
    return std::make_shared<Character>();
}

Character::Character()
: _logicalPosX(0.0f)
, _logicalPosY(0.0f)
{
    Init();
}

void Character::AddAction(CharacterAction &action)
{
    float finishPosX = _logicalPosX + action.GetDeltaX();
    float finishPosY = _logicalPosY + action.GetDeltaX();
    if (!_actionSequence.empty()) {
        // check position by last action
        const CharacterAction &lastAction = _actionSequence.back();
        finishPosX = lastAction.GetFinishX() + action.GetDeltaX();
        finishPosY = lastAction.GetFinishY() + action.GetDeltaY();
    }

    action.SetFinishPosX(finishPosX);
    action.SetFinishPosY(finishPosY);
    _actionSequence.push(action);
}

void Character::AddHealth(float health)
{
    _healthPoints += health;
}

float Character::GetRunningSpeed() const
{
    return _runningSpeed;
}

float Character::GetLogicalX() const
{
    return _logicalPosX;
}

float Character::GetLogicalY() const
{
    return _logicalPosY;
}

CharacterAction& Character::CurrentAction()
{
    return _actionSequence.front();
}

void Character::FinishCurrentAction()
{
    _actionSequence.pop();
}

void Character::SetLogicalPos(float x, float y)
{
    _logicalPosX = x;
    _logicalPosY = y;
}

bool Character::IsAlive() const
{
    return _healthPoints > 0.0f;
}

bool Character::IsAbleToPerform(const CharacterAction &action) const
{
    float lastPosX = _logicalPosX;
    float lastPosY = _logicalPosY;
    if (!_actionSequence.empty()) {
        // check position by last action
        const CharacterAction &lastAction = _actionSequence.back();
        lastPosX = lastAction.GetFinishX();
        lastPosY = lastAction.GetFinishY();
    }
    
    if (action.IsType(CharacterAction::Type::SWIPE_RIGHT)) {
        return lastPosX <= GameInfo::Instance().GetFloat("PATH_RIGHT_BORDER");
    }
    if (action.IsType(CharacterAction::Type::SWIPE_LEFT)) {
        return lastPosX >= GameInfo::Instance().GetFloat("PATH_LEFT_BORDER");
    }
    return false;
}

bool Character::IsActionsQueueFull() const
{
    return _actionSequence.size() >= _actionsSequenceMaxSize;
}

bool Character::HasActionToPerform() const
{
    return !_actionSequence.empty();
}

void Character::Init()
{
    GameInfo &gameinfo = GameInfo::Instance();
    
    _runningSpeed = gameinfo.GetFloat("CHARACTER_RUNNING_SPEED");
    _attackDamage = gameinfo.GetFloat("CHARACTER_ATTACK_DAMAGE");
    _healthPoints = gameinfo.GetFloat("CHARACTER_HEALTH_POINTS");
    _actionsSequenceMaxSize = gameinfo.GetInt("CHARACTER_ACTIONS_SEQUENCE_SIZE");
}
