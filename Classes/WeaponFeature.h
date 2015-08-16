//
//  WeaponFeature.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 11/06/15.
//
//

#ifndef __SlashingTrough__WeaponFeature__
#define __SlashingTrough__WeaponFeature__

#include "EquipFeature.h"

class Hero;
class Entity;

class WeaponFeature : public EquipFeature
{
public:
    WeaponFeature();
    
    virtual void onHit(Entity *goal);
    virtual void onKill(Entity *goal);
};

class CoinsForMurder : public WeaponFeature
{
public:
    static Ptr create(int flat, int percentOfEnemyCost);
    
public:
    CoinsForMurder(int flag, int percentOfEnemyCost);
    
    void onKill(Entity *goal) override;
    
private:
    const int _flat;
    const int _percentOfEnemyCost;
};

#endif /* defined(__SlashingTrough__WeaponFeature__) */
