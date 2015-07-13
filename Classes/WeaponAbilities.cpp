//
//  WeaponAbilities.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 11/06/15.
//
//

#include "WeaponAbilities.h"
#include "Hero.h"

WeaponAbility::~WeaponAbility()
{
}

void WeaponAbility::init(Hero *hero)
{
    _hero = hero;
}

void WeaponAbility::onHit(Entity *goal)
{
}

void WeaponAbility::onKill(Entity *goal)
{
}

WeaponAbility::Ptr CoinsForMurder::Create(int flat, int percentOfEnemyCost)
{
    return std::make_shared<CoinsForMurder>(flat, percentOfEnemyCost);
}

CoinsForMurder::CoinsForMurder(int flag, int percentOfEnemyCost)
: WeaponAbility()
, _flat(flag)
, _percentOfEnemyCost(percentOfEnemyCost)
{
}

void CoinsForMurder::onKill(Entity *goal)
{
    if (!_hero || !goal) {
        return;
    }
    
    Reward *reward = dynamic_cast<Reward *>(goal);
    if (reward) {
        int additional = reward->getCoinPoints() * _percentOfEnemyCost / 100;
        _hero->addCoinsPoint(_flat + additional);
    }
}
