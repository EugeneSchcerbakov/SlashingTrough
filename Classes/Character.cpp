//
//  Character.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 06/04/15.
//
//

#include "Character.h"

#include "GameInfo.h"

GameplayObject::Ptr Character::Create()
{
    return std::make_shared<Character>();
}

Character* Character::Cast(GameplayObject::Ptr object)
{
    return static_cast<Character *>(object.get());
}

Character::Character()
: GameplayObject(Type::CHARACTER, InvalidUID)
, _killPointToNextDamageUp(0)
, _killPoints(0)
{
    Init();
}

void Character::AddAction(CharacterAction &action)
{
    float finishPosX = _logicalX + action.GetDeltaX();
    float finishPosY = _logicalY + action.GetDeltaX();
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

void Character::AddKillPoint(int killPoint)
{
    _killPoints += killPoint;
    _killPointToNextDamageUp += killPoint;
    if (_killPointToNextDamageUp >= _damageUpKillPoints) {
        _killPointToNextDamageUp = 0;
        _damage += _damageUpValue;
    }
}

CharacterAction& Character::CurrentAction()
{
    return _actionSequence.front();
}

void Character::FinishCurrentAction()
{
    _actionSequence.pop();
}

bool Character::IsAbleToPerform(const CharacterAction &action) const
{
    float lastPosX = _logicalX;
    float lastPosY = _logicalY;
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
    
    _damage = gameinfo.GetFloat("CHARACTER_ATTACK_DAMAGE");
    _radius = gameinfo.GetFloat("CHARACTER_ATTACK_DISTANCE");
    _health = gameinfo.GetFloat("CHARACTER_HEALTH_POINTS");
    _damageUpValue = gameinfo.GetFloat("CHARACTER_DAMAGE_UP_VALUE");
    _damageUpKillPoints = gameinfo.GetInt("CHARACTER_DAMAGE_UP_KP");
    _actionsSequenceMaxSize = gameinfo.GetInt("CHARACTER_ACTIONS_SEQUENCE_SIZE");
}

