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

void Equip::featuresInit(Hero *owner)
{
    for (auto f : features) {
        f->init(owner);
    }
}

void Equip::featuresUpdate(float dt)
{
    for (auto f : features) {
        f->update(dt);
    }
}

void Equip::featuresOnSwipeRight()
{
    for (auto f : features) {
        f->onSwipeRight();
    }
}

void Equip::featuresOnSwipeLeft()
{
    for (auto f : features) {
        f->onSwipeLeft();
    }
}

void Equip::featuresOnSwipeBack()
{
    for (auto f : features) {
        f->onSwipeBack();
    }
}

void Equip::setSold(bool flag)
{
    sold = flag;
}

int Equip::getPrice() const
{
    return price;
}

Equip::Type Equip::getType() const
{
    return type;
}

const std::string& Equip::getId() const
{
    return id;
}

const std::string& Equip::getDesc() const
{
    return desc;
}

const std::string& Equip::getIcon() const
{
    return icon;
}

const std::string& Equip::getName() const
{
    return name;
}

bool Equip::isSold() const
{
    return sold;
}

bool Equip::isType(Equip::Type t) const
{
    return type == t;
}

// EquipWeapon

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

void EquipWeapon::featuresOnHit(Entity *goal)
{
    for (auto f : features) {
        auto feature = dynamic_cast<WeaponFeature *>(f.get());
        if (feature) {
            feature->onHit(goal);
        }
    }
}
    
void EquipWeapon::featuresOnKill(Entity *goal)
{
    for (auto f : features) {
        auto feature = dynamic_cast<WeaponFeature *>(f.get());
        if (feature) {
            feature->onKill(goal);
        }
    }
}

float EquipWeapon::getDamage() const
{
    return damage;
}

float EquipWeapon::getSpeed() const
{
    return speed;
}

float EquipWeapon::getDistance() const
{
    return distance;
}

const std::string& EquipWeapon::getSprite() const
{
    return sprite;
}

const EquipWeapon::TrailInfo& EquipWeapon::getTrailInfo() const
{
    return trail;
}

// EquipArmor

Equip::Ptr EquipArmor::create()
{
    return std::make_shared<EquipArmor>();
}

EquipArmor* EquipArmor::cast(Equip::Ptr base)
{
    return dynamic_cast<EquipArmor *>(base.get());
}

EquipArmor::EquipArmor()
: Equip(Type::ARMOR)
, addHealth(0.0f)
{
}

float EquipArmor::getExtraHealth() const
{
    return addHealth;
}

const std::string& EquipArmor::getSprite() const
{
    return sprite;
}
