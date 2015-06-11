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

void WeaponAbility::Init(GameplayObject::WeakPtr hero)
{
    _hero = hero;
}

void WeaponAbility::OnAttackStarted()
{
}

void WeaponAbility::OnAttackEnded()
{
}

void WeaponAbility::OnHit(GameplayObject::WeakPtr enemy)
{
}

void WeaponAbility::OnKill(GameplayObject::WeakPtr enemy)
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

void CoinsForMurder::Init(GameplayObject::WeakPtr hero)
{
    WeaponAbility::Init(hero);
}

void CoinsForMurder::OnAttackStarted()
{
    
}

void CoinsForMurder::OnAttackEnded()
{
    
}

void CoinsForMurder::OnHit(GameplayObject::WeakPtr enemy)
{
    
}

void CoinsForMurder::OnKill(GameplayObject::WeakPtr enemy)
{
    Hero *heroPtr = Hero::Cast(_hero.lock());
    GameplayObject::Ptr enemyPtr = enemy.lock();
    
    int additional = enemyPtr->GetRewardGoldPoints() * _percentOfEnemyCost / 100;
    heroPtr->AddGoldPoints(_flat + additional);
}
