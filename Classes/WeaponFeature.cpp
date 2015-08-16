//
//  WeaponFeature.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 11/06/15.
//
//

#include "WeaponFeature.h"
#include "Hero.h"

WeaponFeature::WeaponFeature()
: EquipFeature()
{
}

void WeaponFeature::onHit(Entity *goal)
{
}

void WeaponFeature::onKill(Entity *goal)
{
}

// CoinsForMurder

EquipFeature::Ptr CoinsForMurder::create(int flat, int percentOfEnemyCost)
{
    return std::make_shared<CoinsForMurder>(flat, percentOfEnemyCost);
}

CoinsForMurder::CoinsForMurder(int flag, int percentOfEnemyCost)
: WeaponFeature()
, _flat(flag)
, _percentOfEnemyCost(percentOfEnemyCost)
{
}

void CoinsForMurder::onKill(Entity *goal)
{
    if (!_owner || !goal) {
        return;
    }
    
    Reward *reward = dynamic_cast<Reward *>(goal);
    if (reward) {
        int additional = reward->getCoinPoints() * _percentOfEnemyCost / 100;
        _owner->addCoinsPoint(_flat + additional);
    }
}
