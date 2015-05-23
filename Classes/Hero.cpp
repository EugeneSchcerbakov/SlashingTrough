//
//  Hero.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 06/04/15.
//
//

#include "Hero.h"

#include "GameInfo.h"
#include "Utils.h"

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
, _staminaDrainTimeCounter(0.0f)
, _runningSpeed(0.0f)
, _posOnRoadX(0.0f)
, _posOnRoadY(0.0f)
, _jumpBackDuration(0.0f)
, _jumpingBackTime(0.0f)
, _jumpStartPos(0.0f)
, _jumpEndPos(0.0f)
, _jumpingBack(false)
{
    Init();
    FlushAllRewards();
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

void Hero::SetPosOnRoad(float x, float y)
{
    _posOnRoadX = x;
    _posOnRoadY = y;
}

void Hero::SetRunningSpeed(float speed)
{
    _runningSpeed = speed;
}

HeroAction& Hero::CurrentAction()
{
    return _actionSequence.front();
}

void Hero::IdleUpdate(float dt)
{
    if (_jumpingBack) {
        _jumpingBackTime += dt * (1.0f / _jumpBackDuration);
        _posOnRoadY = math::lerp(_jumpStartPos, _jumpEndPos, _jumpingBackTime);
        if (_jumpingBackTime >= 1.0f) {
            _jumpingBackTime = 0.0f;
            _jumpingBack = 0.0f;
        }
    } else {
        _posOnRoadY += _runningSpeed * dt;
    }
    
    _staminaDrainTimeCounter += dt;
    if (_staminaDrainTimeCounter >= _staminaDrainTime) {
        _staminaDrainTimeCounter = 0.0f;
        AddStaminaPoints(-_staminaDrainValue);
    }
    
    if (_staminaPoints <= 0.0f) {
        _staminaPoints = 0.0f;
        Kill();
    }
}

void Hero::JumpBack(float duration, float distance)
{
    _jumpBackDuration = duration;
    _jumpStartPos = _posOnRoadY;
    _jumpEndPos = _posOnRoadY - distance;
    _jumpingBackTime = 0.0f;
    _jumpingBack = true;
}

void Hero::FlushAllRewards()
{
    _killPoints = 0;
    _goldPoints = 0;
    _scorePoints = 0;
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
    } else if (action.IsType(HeroAction::Type::SWIPE_LEFT)) {
        return lastPosX >= GameInfo::Instance().GetFloat("PATH_LEFT_BORDER");
    } else if (action.IsType(HeroAction::Type::JUMP_BACK)) {
        return true;
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

void Hero::AddKillPoints(int killPoints)
{
    _killPoints += killPoints;
    _killPointToNextDamageUp += killPoints;
    if (_killPointToNextDamageUp >= _damageUpKillPoints) {
        _killPointToNextDamageUp = 0;
        _damage += _damageUpValue;
    }
}

void Hero::AddStaminaPoints(float staminaPoints)
{
    _staminaPoints += staminaPoints;
    
    float max = GameInfo::Instance().GetFloat("HERO_STAMINA_POINTS");
    if (_staminaPoints > max) {
        _staminaPoints = max;
    }
    if (_staminaPoints < 0.0f) {
        _staminaPoints = 0.0f;
    }
}

void Hero::AddGoldPoints(int goldPoints)
{
    _goldPoints += goldPoints;
}

void Hero::AddScorePoints(int scorePoints)
{
    _scorePoints += scorePoints;
}

float Hero::GetXPosOnRoad() const
{
    return _posOnRoadX;
}

float Hero::GetYPosOnRoad() const
{
    return _posOnRoadY;
}

float Hero::GetRunningSpeed() const
{
    return _runningSpeed;
}

float Hero::GetStaminaPoints() const
{
    return _staminaPoints;
}

int Hero::GetKillPoints() const
{
    return _killPoints;
}

int Hero::GetGoldPoints() const
{
    return _goldPoints;
}

int Hero::GetScorePoints() const
{
    return _scorePoints;
}

int Hero::GetDamagePoints() const
{
    return (int)_damage;
}

void Hero::Init()
{
    GameInfo &gameinfo = GameInfo::Instance();
    
    _damage = gameinfo.GetFloat("HERO_ATTACK_DAMAGE");
    _radius = gameinfo.GetFloat("HERO_ATTACK_DISTANCE");
    _health = gameinfo.GetFloat("HERO_HEALTH_POINTS");
    _damageUpValue = gameinfo.GetFloat("HERO_DAMAGE_UP_VALUE");
    _damageUpKillPoints = gameinfo.GetInt("HERO_DAMAGE_UP_KP");
    _staminaPoints = gameinfo.GetFloat("HERO_STAMINA_POINTS");
    _staminaDrainTime = gameinfo.GetFloat("HERO_STAMINA_DRAIN_TIME");
    _staminaDrainValue = gameinfo.GetFloat("HERO_STAMINA_DRAIN_VALUE");
    _actionsSequenceMaxSize = gameinfo.GetInt("HERO_ACTIONS_SEQUENCE_SIZE");
}

