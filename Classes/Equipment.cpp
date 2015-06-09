//
//  Equipment.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 06/06/15.
//
//

#include "Equipment.h"

Equip::Equip(Equip::Type t)
: type(t)
, sold(false)
, id("")
{
}

Equip::~Equip()
{
}

Equip::Ptr EquipWeapon::create()
{
    return std::make_shared<EquipWeapon>();
}

EquipWeapon* EquipWeapon::cast(Equip::Ptr base)
{
    return dynamic_cast<EquipWeapon *>(base.get());
}

EquipWeapon::EquipWeapon()
: Equip(Type::WEAPON)
, damage(0.0f)
, speed(0.0f)
, distance(0.0f)
{
}