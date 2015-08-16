//
//  EquipFeature.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 15/08/15.
//
//

#include "EquipFeature.h"
#include "Hero.h"

EquipFeature::EquipFeature()
: _owner(nullptr)
{
}

EquipFeature::~EquipFeature()
{
}

void EquipFeature::init(Hero *owner)
{
    _owner = owner;
}

void EquipFeature::update(float dt)
{
}

void EquipFeature::onSwipeBack()
{
}

void EquipFeature::onSwipeRight()
{
}

void EquipFeature::onSwipeLeft()
{
}

