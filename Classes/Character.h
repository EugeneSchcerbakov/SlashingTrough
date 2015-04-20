//
//  Character.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 06/04/15.
//
//

#ifndef __SlashingTrough__Character__
#define __SlashingTrough__Character__

#include <memory>

#include "CharacterAction.h"
#include "PathSector.h"

class Character
{
public:
    typedef std::shared_ptr<Character> Ptr;
    typedef std::weak_ptr<Character> WeakPtr;
    
public:
    static Ptr Create();
    
    Character();
    
    void Attack(GameplayObject::Ptr object, float distance);
    void FinishCurrentAction();
    
    void SetLogicalPos(float x, float y);
    
    void AddAction(CharacterAction &action);
    void AddHealth(float health);
    
    float GetRunningSpeed() const;
    float GetLogicalX() const;
    float GetLogicalY() const;
    CharacterAction& CurrentAction();
    
    bool IsAlive() const;
    bool IsAbleToPerform(const CharacterAction &action) const;
    bool IsActionsQueueFull() const;
    bool HasActionToPerform() const;
    
protected:
    typedef std::queue<CharacterAction> ActionSequence;
    
protected:
    virtual void Init();
    
    ActionSequence _actionSequence;
    
    float _runningSpeed;
    float _attackDamage;
    float _attackDistance;
    float _healthPoints;
    float _logicalPosX;
    float _logicalPosY;
    int _actionsSequenceMaxSize;
};

#endif /* defined(__SlashingTrough__Character__) */
