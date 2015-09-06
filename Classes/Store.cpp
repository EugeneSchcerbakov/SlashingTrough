//
//  Store.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 01/06/15.
//
//

#include "Store.h"
#include "PlayerInfo.h"
#include "Log.h"

#include "cocos2d.h"

Store& Store::getInstance()
{
    static Store singleInstance;
    return singleInstance;
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
                //auto featuresRoot = elem->FirstChildElement("Features");
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
                //parseEquipFeature(item, featuresRoot);
                _items.push_back(item);
            } else if (name == "Armor") {
                //auto featuresRoot = elem->FirstChildElement("Features");
                Item::Ptr item = ItemArmor::create();
                ItemArmor *armor = ItemArmor::cast(item);
                armor->id = elem->Attribute("id");
                armor->price = elem->IntAttribute("price");
                armor->addHealth = elem->FloatAttribute("addHealth");
                armor->desc = elem->Attribute("desc");
                armor->icon = elem->Attribute("icon");
                armor->sprite = elem->Attribute("sprite");
                armor->name = elem->Attribute("name");
                //parseEquipFeature(item, featuresRoot);
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

/*
void Store::parseEquipFeature(Equip::WeakPtr equip, tinyxml2::XMLElement *root)
{
    if (equip.expired() || !root) {
        return;
    }
    
    auto equip_ptr = equip.lock();
    auto elem = root->FirstChildElement();
    
    while (elem) {
        std::string name = elem->Name();
        if (name == "CoinsForMurder") {
            int flat = elem->IntAttribute("flat");
            int percent = elem->IntAttribute("percentOfEnemyCost");
            auto feature = CoinsForMurder::create(flat, percent);
            equip_ptr->features.push_back(feature);
        } else if (name == "Backsliding") {
            float cooldown = elem->FloatAttribute("cooldown");
            float distance = elem->FloatAttribute("distance");
            float duration = elem->FloatAttribute("duration");
            auto feature = Backsliding::create(cooldown, distance, duration);
            equip_ptr->features.push_back(feature);
        } else if (name == "BackslidingShield") {
            float cooldown = elem->FloatAttribute("cooldown");
            float distance = elem->FloatAttribute("distance");
            float duration = elem->FloatAttribute("duration");
            float shieldTime = elem->FloatAttribute("shieldTime");
            auto feature = BackslidingShield::create(cooldown, distance, duration, shieldTime);
            equip_ptr->features.push_back(feature);
        } else {
            CC_ASSERT(false);
        }
        elem = elem->NextSiblingElement();
    }
}
*/

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

Store::Items Store::getWeaponItems() const
{
    Items items;
    for (auto item : _items) {
        if (item->type == Item::Type::WEAPON) {
            items.push_back(item);
        }
    }
    return items;
}

Store::Items Store::getArmorItems() const
{
    Items items;
    for (auto item : _items) {
        if (item->type == Item::Type::ARMOR) {
            items.push_back(item);
        }
    }
    return items;
}

void Store::reset()
{
    _items.clear();
}
