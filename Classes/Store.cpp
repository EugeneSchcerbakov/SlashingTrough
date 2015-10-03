//
//  Store.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 01/06/15.
//
//

#include "Store.h"
#include "PlayerInfo.h"
#include "Abilities.h"
#include "Log.h"

#include "cocos2d.h"

Store& Store::getInstance()
{
    static Store singleInstance;
    return singleInstance;
}

static Ability::Ptr createAbility(tinyxml2::XMLElement *elem)
{
    if (!elem) {
        WRITE_ERR("Ability xml description is missed.");
        return Ability::Ptr();
    }
    
    Ability::Ptr ability;
    
    std::string name = elem->Attribute("name");
    if (name == "BackDash") {
        float cooldown = elem->FloatAttribute("cooldown");
        float distance = elem->FloatAttribute("distance");
        float duration = elem->FloatAttribute("duration");
        ability = BackDash::create(cooldown, distance, duration);
    } else if (name == "BackDashShield") {
        float cooldown = elem->FloatAttribute("cooldown");
        float distance = elem->FloatAttribute("distance");
        float duration = elem->FloatAttribute("duration");
        float shieldTime = elem->FloatAttribute("shieldTime");
        ability = BackDashShield::create(cooldown, distance, duration, shieldTime);
    } else if (name == "Crit") {
        float mult = elem->FloatAttribute("damage");
        int chance = elem->IntAttribute("chance");
        ability = Crit::create(mult, chance);
    } else if (name == "Vampirism") {
        float drain = elem->FloatAttribute("healthDrain");
        float returns = elem->FloatAttribute("healthReturn");
        ability = Vampirism::create(drain, returns);
    } else if (name == "BurningAura") {
        float damage = elem->FloatAttribute("damage");
        float frequency =  elem->FloatAttribute("frequency");
        float radius = elem->FloatAttribute("radius");
        ability = BurningAura::create(damage, frequency, radius);
    } else if (name == "ExtendedRange") {
        float increase = elem->FloatAttribute("increase");
        ability = ExtendedRange::create(increase);
    } else if (name == "Regeneration") {
        float healthPerSec = elem->FloatAttribute("healthPerSec");
        ability = Regeneration::create(healthPerSec);
    } else if (name == "MagicShield") {
        float absorb = elem->FloatAttribute("absorb");
        ability = MagicShield::create(absorb);
    } else if (name == "ProjectileAbsorb") {
        ability = ProjectileAbsorb::create();
    } else if (name == "ProjectileReflect") {
        ability = ProjectileReflect::create();
    } else {
        WRITE_WARN("Unknown ability.");
    }
    
    return ability;
}

void Store::loadStore(const std::string &filename)
{
    reset();
    
    cocos2d::FileUtils *fileUtils = cocos2d::FileUtils::getInstance();
    std::string path = fileUtils->fullPathForFilename(filename);
    std::string buff = fileUtils->getStringFromFile(path);
    
    tinyxml2::XMLDocument document;
    tinyxml2::XMLError result = document.Parse(buff.c_str());
    
    if (result == tinyxml2::XMLError::XML_SUCCESS || result == tinyxml2::XMLError::XML_NO_ERROR)
    {
        tinyxml2::XMLNode *root = document.RootElement();
        tinyxml2::XMLNode *node = root->FirstChild();
        while (node) {
            tinyxml2::XMLElement *elem = node->ToElement();
            std::string name = elem->Name();
            if (name == "Weapon") {
                auto trailInfo = elem->FirstChildElement("Trail");
                Item::Ptr item = ItemWeapon::create();
                ItemWeapon *weapon = ItemWeapon::cast(item);
                weapon->id = elem->Attribute("id");
                weapon->price = elem->IntAttribute("price");
                weapon->damage = elem->FloatAttribute("damage");
                weapon->speed = elem->FloatAttribute("speed");
                weapon->distance = elem->FloatAttribute("distance");
                weapon->desc = elem->Attribute("desc");
                weapon->icon = elem->Attribute("icon");
                weapon->sprite = elem->Attribute("sprite");
                weapon->name = elem->Attribute("name");
                if (trailInfo) {
                    weapon->trail.length = trailInfo->FloatAttribute("length");
                    weapon->trail.width = trailInfo->FloatAttribute("width");
                    weapon->trail.posYCoeff = trailInfo->FloatAttribute("posYCoeff");
                    weapon->trail.opacity = trailInfo->FloatAttribute("opacity");
                    weapon->trail.texture = trailInfo->Attribute("texture");
                }
                _items.push_back(item);
            } else if (name == "Armor") {
                Item::Ptr item = ItemArmor::create();
                ItemArmor *armor = ItemArmor::cast(item);
                armor->id = elem->Attribute("id");
                armor->price = elem->IntAttribute("price");
                armor->addHealth = elem->FloatAttribute("addHealth");
                armor->desc = elem->Attribute("desc");
                armor->icon = elem->Attribute("icon");
                armor->sprite = elem->Attribute("sprite");
                armor->name = elem->Attribute("name");
                _items.push_back(item);
            } else if (name == "Crystall") {
                Item::Ptr item = Crystall::create();
                Crystall *crystall = Crystall::cast(item);
                crystall->id = elem->Attribute("id");
                crystall->name = elem->Attribute("name");
                
                std::string kind = elem->Attribute("kind");
                if (kind == "weapon") {
                    crystall->kind = Crystall::Kind::WEAPON;
                } else if (kind == "armor") {
                    crystall->kind = Crystall::Kind::ARMOR;
                } else {
                    WRITE_WARN("Unknown crystall kind: " + kind);
                }
                
                // find tiers info
                int tierIndex = 1;
                std::string query = "Tier" + cocos2d::StringUtils::toString(tierIndex);
                auto tier = elem->FirstChildElement(query.c_str());
                while (tier)
                {
                    Crystall::TierData data;
                    data.shards = tier->IntAttribute("shards");
                    data.icon = tier->Attribute("icon");
                    data.desc = tier->Attribute("desc");
                    data.ability = createAbility(tier->FirstChildElement("Ability"));
                    crystall->tiersData.push_back(data);
                    
                    tierIndex++;
                    query = "Tier" + cocos2d::StringUtils::toString(tierIndex);
                    tier = elem->FirstChildElement(query.c_str());
                }
                _items.push_back(item);
            } else if (name == "Shard") {
                Item::Ptr item = Crystall::Shard::create();
                Crystall::Shard *shard = Crystall::Shard::cast(item);
                shard->id = elem->Attribute("id");
                shard->icon = elem->Attribute("icon");
                _items.push_back(item);
            } else {
                WRITE_WARN("Unknown item in store description");
                CC_ASSERT(false);
            }
            node = node->NextSibling();
        }
        WRITE_INIT("Store description loaded successfully.");
    }
    else
    {
        WRITE_ERR("Failed to load score description.");
    }
}

bool Store::buy(const std::string &id)
{
    // don't use store in the ctor, in may no be loaded yet
    PlayerInfo &player = PlayerInfo::getInstance();
    
    Item::Ptr equip = getItemById(id);
    if (equip && equip->price <= player.getCoins())
    {
        player.addCoins(-equip->price);
        player.Inventory.add(equip);
        WRITE_LOG("Bought item " + id);
        return true;
    }
    
    return false;
}

Item::Ptr Store::getItemById(const std::string &id) const
{
    for (const auto info : _items) {
        if (info->id == id) {
            return info;
        }
    }
    WRITE_WARN("Invalid item required: " + id);
    return Item::Ptr();
}

const Store::Items& Store::getAllItems() const
{
    return _items;
}

Store::Items Store::getItemsWithType(Item::Type type) const
{
    Items items;
    for (auto item : _items) {
        if (item->isType(type)) {
            items.push_back(item);
        }
    }
    return items;
}

void Store::reset()
{
    _items.clear();
}
