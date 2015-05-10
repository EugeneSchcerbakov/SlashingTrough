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

class Hero : public GameplayObject
{
public:
    typedef std::shared_ptr<Hero> Ptr;
    typedef std::weak_ptr<Hero> WeakPtr;
    
public:
    static GameplayObject::Ptr Create();
    static Hero* Cast(GameplayObject::Ptr object);
    
    Hero();
    
    void FinishCurrentAction();
    void AddAction(HeroAction &action);
    void AddKillPoint(int killPoitns = 1);
    
    HeroAction& CurrentAction();
    
    bool IsAbleToPerform(const HeroAction &action) const;
    bool IsActionsQueueFull() const;
    bool HasActionToPerform() const;
    
protected:
    typedef std::queue<HeroAction> ActionSequence;
    
protected:
    virtual void Init();
    
    ActionSequence _actionSequence;
    
    float _attackDistance;
    float _damageUpValue;
    
    int _killPointToNextDamageUp;
    int _damageUpKillPoints;
    int _actionsSequenceMaxSize;
    
    int _killPoints;
};

#endif /* defined(__SlashingTrough__Hero__) */
