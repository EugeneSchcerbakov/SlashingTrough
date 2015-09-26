//
//  Item.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 06/06/15.
//
//

#include "Item.h"

#include "PlayerInfo.h"
#include "Hero.h"
#include "Utils.h"
#include "Log.h"

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

// Ability

Ability::Ability()
: _hero(nullptr)
{
}

void Ability::init(Hero *p)
{
    _hero = p;
}

void Ability::update(float dt)
{
    if (!_hero) {WRITE_ERR("Failed to perform alility with invalid owner.");}
}

void Ability::swipeLeft()
{
    if (!_hero) {WRITE_ERR("Failed to perform alility with invalid owner.");}
}

void Ability::swipeRight()
{
    if (!_hero) {WRITE_ERR("Failed to perform alility with invalid owner.");}
}

void Ability::swipeBack()
{
    if (!_hero) {WRITE_ERR("Failed to perform alility with invalid owner.");}
}

void Ability::swipeForward()
{
    if (!_hero) {WRITE_ERR("Failed to perform alility with invalid owner.");}
}

void Ability::hit(Entity *e)
{
    if (!_hero) {WRITE_ERR("Failed to perform alility with invalid owner.");}
}

void Ability::kill(Entity *e)
{
    if (!_hero) {WRITE_ERR("Failed to perform alility with invalid owner.");}
}

void Ability::damage()
{
    if (!_hero) {WRITE_ERR("Failed to perform alility with invalid owner.");}
}

// Crystall

Item::Ptr Crystall::Shard::create()
{
    return std::make_shared<Crystall::Shard>();
}

Crystall::Shard* Crystall::Shard::cast(Item::Ptr base)
{
    return dynamic_cast<Crystall::Shard *>(base.get());
}

Crystall::Shard::Shard()
: Item(Type::SHARD)
{
}

Item::Ptr Crystall::create()
{
    return std::make_shared<Crystall>();
}

Crystall* Crystall::cast(Item::Ptr base)
{
    return dynamic_cast<Crystall *>(base.get());
}

const int Crystall::LockTier = -1;

Crystall::Crystall()
: Item(Type::CRYSTALL)
, kind(Kind::NONE)
, tier(LockTier)
, _hero(nullptr)
{
}

void Crystall::onInit(Hero *hero)
{
    _hero = hero;
    
    if (isUnlocked()) {
        tiersData[tier].ability->init(hero);
    }
}

void Crystall::onUpdate(float dt)
{
    if (isUnlocked()) {
        tiersData[tier].ability->update(dt);
    }
}

void Crystall::onSwipeLeft()
{
    if (isUnlocked()) {
        tiersData[tier].ability->swipeLeft();
    }
}

void Crystall::onSwipeRight()
{
    if (isUnlocked()) {
        tiersData[tier].ability->swipeRight();
    }
}

void Crystall::onSwipeBack()
{
    if (isUnlocked()) {
        tiersData[tier].ability->swipeBack();
    }
}

void Crystall::onSwipeForward()
{
    if (isUnlocked()) {
        tiersData[tier].ability->swipeForward();
    }

}

void Crystall::onHit(Entity *entity)
{
    if (isUnlocked()) {
        tiersData[tier].ability->hit(entity);
    }
}

void Crystall::onKill(Entity *entity)
{
    if (isUnlocked()) {
        tiersData[tier].ability->kill(entity);
    }
}

void Crystall::onDamage()
{
    if (isUnlocked()) {
        tiersData[tier].ability->damage();
    }
}

const std::string& Crystall::getDesc() const
{
    if (tier >= LockTier) {
        int max = (int)tiersData.size() - 1;
        int index = math::clamp(tier+1, 0, max);
        return tiersData[index].desc;
    } else {
        WRITE_WARN("Invalid tier index");
        return desc;
    }
}

const std::string& Crystall::getIcon() const
{
    if (tier >= LockTier) {
        int max = (int)tiersData.size() - 1;
        int index = math::clamp(tier+1, 0, max);
        return tiersData[index].icon;
    } else {
        WRITE_WARN("Invalid tier index");
        return icon;
    }
}

void Crystall::refreshState()
{
    PlayerInfo &player = PlayerInfo::getInstance();
    std::string query = getShardId();
    int totalShards = player.Inventory.getAmount(query);
    
    tier = LockTier;
    
    for (const auto &tierInfo : tiersData)
    {
        totalShards -= tierInfo.shards;
        if (totalShards >= 0) {
            nextTier();
        } else {
            break;
        }
    }
}

void Crystall::nextTier()
{
    int newTier = tier + 1;
    
    if (newTier > LockTier && newTier < getMaxTier()) {
        tier = newTier;
    } else {
        WRITE_WARN("Trying to set invalid crystall tier.");
    }
}

Crystall::TierData Crystall::getCurrentTierData() const
{
    if (tier >= LockTier && tier < (int)tiersData.size()) {
        int max = (int)tiersData.size() - 1;
        int index = math::clamp(tier+1, 0, max);
        return tiersData[index];
    } else {
        WRITE_WARN("Accessing invalid tier.");
        return TierData();
    }
}

int Crystall::getShardsToNextTier() const
{
    if (tier < getMaxTier()) {
        PlayerInfo &player = PlayerInfo::getInstance();
        std::string query = getShardId();
        int found = player.Inventory.getAmount(query);
        int used = 0;
        for (int k=0;k<=tier;k++) {
            used += tiersData[k].shards;
        }
        int delta = std::max(0, found - used);
        return delta;
    } else {
        return 0;
    }
}

std::string Crystall::getShardId() const
{
    std::string query = id;
    std::string from = "crystall";
    std::string to = "shard";
    
    std::size_t index = query.find(from);
    
    if (index == std::string::npos) {
        WRITE_WARN("Failed to find particular shard.");
        return "";
    }
    
    query.replace(index, from.length(), to);
    
    return query;
}

int Crystall::getMaxTier() const
{
    return tiersData.size();
}

int Crystall::getTier() const
{
    return tier;
}

Crystall::Kind Crystall::getKind() const
{
    return kind;
}

bool Crystall::isKind(Crystall::Kind which) const
{
    return kind == which;
}

bool Crystall::isUnlocked() const
{
    return tier >= 0;
}
