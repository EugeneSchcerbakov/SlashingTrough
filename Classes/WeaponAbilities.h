//
//  WeaponAbilities.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 11/06/15.
//
//

#ifndef __SlashingTrough__WeaponAbilities__
#define __SlashingTrough__WeaponAbilities__

#include <memory>
#include "GameplayObjects.h"

class WeaponAbility
{
public:
    typedef std::shared_ptr<WeaponAbility> Ptr;
    typedef std::weak_ptr<WeaponAbility> WeakPtr;
    
public:
    virtual ~WeaponAbility();
    
    virtual void Init(GameplayObject::WeakPtr hero);
    virtual void OnAttackStarted();
    virtual void OnAttackEnded();
    virtual void OnHit(GameplayObject::WeakPtr enemy);
    virtual void OnKill(GameplayObject::WeakPtr enemy);
    
protected:
    GameplayObject::WeakPtr _hero;
};

class CoinsForMurder : public WeaponAbility
{
public:
    static WeaponAbility::Ptr Create(int flat, int percentOfEnemyCost);
    
    CoinsForMurder(int flag, int percentOfEnemyCost);
    
    void Init(GameplayObject::WeakPtr hero) override;
    void OnAttackStarted() override;
    void OnAttackEnded() override;
    void OnHit(GameplayObject::WeakPtr enemy) override;
    void OnKill(GameplayObject::WeakPtr enemy) override;
    
private:
    const int _flat;
    const int _percentOfEnemyCost;
};

#endif /* defined(__SlashingTrough__WeaponAbilities__) */
