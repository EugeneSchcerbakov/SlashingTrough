//
//  Hero.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 06/04/15.
//
//

#include "Hero.h"

#include "GameInfo.h"

GameplayObject::Ptr Hero::Create()
{
    return std::make_shared<Hero>();
}

Hero* Hero::Cast(GameplayObject::Ptr object)
{
    return static_cast<Hero *>(object.get());
}

Hero::Hero()
: GameplayObject(Type::HERO, InvalidUID)
, _killPointToNextDamageUp(0)
, _killPoints(0)
{
    Init();
}

void Hero::AddAction(HeroAction &action)
{
    float finishPosX = _logicalX + action.GetDeltaX();
    float finishPosY = _logicalY + action.GetDeltaX();
    if (!_actionSequence.empty()) {
        // check position by last action
        const HeroAction &lastAction = _actionSequence.back();
        finishPosX = lastAction.GetFinishX() + action.GetDeltaX();
        finishPosY = lastAction.GetFinishY() + action.GetDeltaY();
    }

    action.SetFinishPosX(finishPosX);
    action.SetFinishPosY(finishPosY);
    _actionSequence.push(action);
}

void Hero::AddKillPoint(int killPoint)
{
    _killPoints += killPoint;
    _killPointToNextDamageUp += killPoint;
    if (_killPointToNextDamageUp >= _damageUpKillPoints) {
        _killPointToNextDamageUp = 0;
        _damage += _damageUpValue;
    }
}

HeroAction& Hero::CurrentAction()
{
    return _actionSequence.front();
}

void Hero::FinishCurrentAction()
{
    _actionSequence.pop();
}

bool Hero::IsAbleToPerform(const HeroAction &action) const
{
    float lastPosX = _logicalX;
    float lastPosY = _logicalY;
    if (!_actionSequence.empty()) {
        // check position by last action
        const HeroAction &lastAction = _actionSequence.back();
        lastPosX = lastAction.GetFinishX();
        lastPosY = lastAction.GetFinishY();
    }
    
    if (action.IsType(HeroAction::Type::SWIPE_RIGHT)) {
        return lastPosX <= GameInfo::Instance().GetFloat("PATH_RIGHT_BORDER");
    }
    if (action.IsType(HeroAction::Type::SWIPE_LEFT)) {
        return lastPosX >= GameInfo::Instance().GetFloat("PATH_LEFT_BORDER");
    }
    return false;
}

bool Hero::IsActionsQueueFull() const
{
    return _actionSequence.size() >= _actionsSequenceMaxSize;
}

bool Hero::HasActionToPerform() const
{
    return !_actionSequence.empty();
}

void Hero::Init()
{
    GameInfo &gameinfo = GameInfo::Instance();
    
    _damage = gameinfo.GetFloat("HERO_ATTACK_DAMAGE");
    _radius = gameinfo.GetFloat("HERO_ATTACK_DISTANCE");
    _health = gameinfo.GetFloat("HERO_HEALTH_POINTS");
    _damageUpValue = gameinfo.GetFloat("HERO_DAMAGE_UP_VALUE");
    _damageUpKillPoints = gameinfo.GetInt("HERO_DAMAGE_UP_KP");
    _actionsSequenceMaxSize = gameinfo.GetInt("HERO_ACTIONS_SEQUENCE_SIZE");
}

