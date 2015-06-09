//
//  Hero.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 06/04/15.
//
//

#ifndef __SlashingTrough__Hero__
#define __SlashingTrough__Hero__

#include <memory>
#include <queue>

#include "HeroAction.h"
#include "PathSector.h"
#include "SessionInfo.h"

class Hero : public GameplayObject
{
public:
    typedef std::shared_ptr<Hero> Ptr;
    typedef std::weak_ptr<Hero> WeakPtr;
    
public:
    static GameplayObject::Ptr Create();
    static Hero* Cast(GameplayObject::Ptr object);
    
    Hero();
    
    void IdleUpdate(float dt);
    void JumpBack(float duration, float distance);
    void FlushAllRewards();
    void FinishCurrentAction();
    void AddAction(HeroAction &action);
    void SetWeapon(Equip::WeakPtr weapon);
    void SetPosOnRoad(float x, float y);
    void SetRunningSpeed(float speed);
    
    HeroAction& CurrentAction();
    SessionInfo::Score GetScore() const;
    const EquipWeapon* GetWeapon() const;
    
    bool IsAbleToPerform(const HeroAction &action) const;
    bool IsActionsQueueFull() const;
    bool HasActionToPerform() const;
    
    void AddStaminaPoints(float staminaPoints);
    void AddKillPoints(int killPoints);
    void AddGoldPoints(int goldPoints);
    void AddScorePoints(int scorePoints);
    
    float GetXPosOnRoad() const;
    float GetYPosOnRoad() const;
    float GetRunningSpeed() const;
    float GetStaminaPoints() const;
    int GetDamagePoints() const;
    
protected:
    typedef std::queue<HeroAction> ActionSequence;
    
protected:
    virtual void Init();
    
    ActionSequence _actionSequence;
    SessionInfo::Score _score;
    Equip::WeakPtr _weapon;
    
    float _attackDistance;
    float _damageUpValue;
    float _runningSpeed;
    float _staminaPoints;
    float _staminaDrainTime;
    float _staminaDrainTimeCounter;
    float _staminaDrainValue;
    float _posOnRoadX;
    float _posOnRoadY;
    
    float _jumpingBackTime;
    float _jumpBackDuration;
    float _jumpBackDistance;
    float _jumpStartPos, _jumpEndPos;
    
    int _killPointToNextDamageUp;
    int _damageUpKillPoints;
    int _actionsSequenceMaxSize;
    
    bool _jumpingBack;
};

#endif /* defined(__SlashingTrough__Hero__) */
