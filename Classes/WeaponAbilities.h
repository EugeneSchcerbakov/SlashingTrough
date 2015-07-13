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

class Hero;
class Entity;

class WeaponAbility
{
public:
    typedef std::shared_ptr<WeaponAbility> Ptr;
    typedef std::weak_ptr<WeaponAbility> WeakPtr;
    
public:
    virtual ~WeaponAbility();
    
    virtual void init(Hero *hero);
    virtual void onHit(Entity *goal);
    virtual void onKill(Entity *goal);
    
protected:
    Hero *_hero;
};

class CoinsForMurder : public WeaponAbility
{
public:
    static WeaponAbility::Ptr Create(int flat, int percentOfEnemyCost);
    
    CoinsForMurder(int flag, int percentOfEnemyCost);
    
    void onKill(Entity *goal) override;
    
private:
    const int _flat;
    const int _percentOfEnemyCost;
};

#endif /* defined(__SlashingTrough__WeaponAbilities__) */