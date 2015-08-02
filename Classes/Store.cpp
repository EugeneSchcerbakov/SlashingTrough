//
//  Store.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 01/06/15.
//
//

#include "Store.h"
#include "SessionInfo.h"

#include "cocos2d.h"
#include "tinyxml2/tinyxml2.h"

const std::string Store::DEFAULT_WEAPON_ID = "default_sword";
const std::string Store::DEFAULT_ARMOR_ID = "default_armor";

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
                tinyxml2::XMLElement *trailInfo = elem->FirstChildElement("Trail");
                tinyxml2::XMLElement *abilitiesRoot = elem->FirstChildElement("Abilities");
                Equip::Ptr item = EquipWeapon::create();
                EquipWeapon *weapon = EquipWeapon::cast(item);
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
                if (abilitiesRoot) {
                    tinyxml2::XMLElement *abilityElem = abilitiesRoot->FirstChildElement();
                    while (abilityElem) {
                        std::string abilityName = abilityElem->Name();
                        if (abilityName == "CoinsForMurder") {
                            int flat = abilityElem->IntAttribute("flat");
                            int percent = abilityElem->IntAttribute("percentOfEnemyCost");
                            WeaponAbility::Ptr ability = CoinsForMurder::Create(flat, percent);
                            weapon->abilities.push_back(ability);
                        } else {
                            CC_ASSERT(false);
                        }
                        abilityElem = abilityElem->NextSiblingElement();
                    }
                }
                _items.push_back(item);
            } else if (name == "Armor") {
                Equip::Ptr item = EquipArmor::create();
                EquipArmor *armor = EquipArmor::cast(item);
                armor->id = elem->Attribute("id");
                armor->price = elem->IntAttribute("price");
                armor->addHealth = elem->FloatAttribute("addHealth");
                armor->desc = elem->Attribute("desc");
                armor->icon = elem->Attribute("icon");
                armor->sprite = elem->Attribute("sprite");
                armor->name = elem->Attribute("name");
                _items.push_back(item);
            } else {
                CC_ASSERT(false);
            }
            node = node->NextSibling();
        }
    }
}

void Store::buy(const std::string &id)
{
    // don't use store in the ctor, in may no be loaded yet
    SessionInfo &session = SessionInfo::getInstance();
    
    Equip::Ptr equip = getItemById(id);
    if (equip && equip->price <= session.getCoins())
    {
        session.addCoins(-equip->price);
        session.addOwnedEquip(equip);
        equip->sold = true;
    }
}

Equip::Ptr Store::getItemById(const std::string &id) const
{
    for (const auto info : _items) {
        if (info->id == id) {
            return info;
        }
    }
    return Equip::Ptr();
}

const Store::Items& Store::getAllItems() const
{
    return _items;
}

Store::Items Store::getWeaponItems() const
{
    Items items;
    for (auto item : _items) {
        if (item->type == Equip::Type::WEAPON) {
            items.push_back(item);
        }
    }
    return items;
}

Store::Items Store::getArmorItems() const
{
    Items items;
    for (auto item : _items) {
        if (item->type == Equip::Type::ARMOR) {
            items.push_back(item);
        }
    }
    return items;
}

void Store::reset()
{
    _items.clear();
}
