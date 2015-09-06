//
//  Item.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 06/06/15.
//
//

#include "Item.h"

Item::Item(Item::Type t)
: type(t)
, id("")
{
}

Item::~Item()
{
}

int Item::getPrice() const
{
    return price;
}

Item::Type Item::getType() const
{
    return type;
}

const std::string& Item::getId() const
{
    return id;
}

const std::string& Item::getDesc() const
{
    return desc;
}

const std::string& Item::getIcon() const
{
    return icon;
}

const std::string& Item::getName() const
{
    return name;
}

bool Item::isType(Item::Type t) const
{
    return type == t;
}

// ItemWeapon

Item::Ptr ItemWeapon::create()
{
    return std::make_shared<ItemWeapon>();
}

ItemWeapon* ItemWeapon::cast(Item::Ptr base)
{
    return dynamic_cast<ItemWeapon *>(base.get());
}

ItemWeapon::ItemWeapon()
: Item(Type::WEAPON)
, damage(0.0f)
, speed(0.0f)
, distance(0.0f)
{
}

float ItemWeapon::getDamage() const
{
    return damage;
}

float ItemWeapon::getSpeed() const
{
    return speed;
}

float ItemWeapon::getDistance() const
{
    return distance;
}

const std::string& ItemWeapon::getSprite() const
{
    return sprite;
}

const ItemWeapon::TrailInfo& ItemWeapon::getTrailInfo() const
{
    return trail;
}

// ItemArmor

Item::Ptr ItemArmor::create()
{
    return std::make_shared<ItemArmor>();
}

ItemArmor* ItemArmor::cast(Item::Ptr base)
{
    return dynamic_cast<ItemArmor *>(base.get());
}

ItemArmor::ItemArmor()
: Item(Type::ARMOR)
, addHealth(0.0f)
{
}

float ItemArmor::getExtraHealth() const
{
    return addHealth;
}

const std::string& ItemArmor::getSprite() const
{
    return sprite;
}
