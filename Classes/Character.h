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
#include <queue>

#include "CharacterAction.h"
#include "PathSector.h"

class Character : public GameplayObject
{
public:
    typedef std::shared_ptr<Character> Ptr;
    typedef std::weak_ptr<Character> WeakPtr;
    
public:
    static GameplayObject::Ptr Create();
    static Character* Cast(GameplayObject::Ptr object);
    
    Character();
    
    void FinishCurrentAction();
    void AddAction(CharacterAction &action);
    
    CharacterAction& CurrentAction();
    
    bool IsAbleToPerform(const CharacterAction &action) const;
    bool IsActionsQueueFull() const;
    bool HasActionToPerform() const;
    
protected:
    typedef std::queue<CharacterAction> ActionSequence;
    
protected:
    virtual void Init();
    
    ActionSequence _actionSequence;
    
    float _attackDamage;
    float _attackDistance;
    
    int _actionsSequenceMaxSize;
};

#endif /* defined(__SlashingTrough__Character__) */
