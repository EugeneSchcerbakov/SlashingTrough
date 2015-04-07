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

class Character
{
public:
    typedef std::shared_ptr<Character> Ptr;
    typedef std::weak_ptr<Character> WeakPtr;
    
public:
    static Ptr Create();
    
    Character();
    
    void AddHealth(float health);
    
    float GetRunningSpeed() const;
    bool IsAlive() const;
    
protected:
    virtual void Init();
    
    float _runningSpeed;
    float _attackDamage;
    float _healthPoints;
};

#endif /* defined(__SlashingTrough__Character__) */
