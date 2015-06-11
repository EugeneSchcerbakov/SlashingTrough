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

Store& Store::Instance()
{
    static Store singleInstance;
    return singleInstance;
}

void Store::LoadStore(const std::string &filename)
{
    Reset();
    
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
                tinyxml2::XMLElement *abilitiesRoot = elem->FirstChildElement("Abilities");
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
            } else {
                CC_ASSERT(false);
            }
            node = node->NextSibling();
        }
    }
    
    SessionInfo &profile = SessionInfo::Instance();
    
    std::string equippedWeaponId = profile.GetEquippedWeaponId();
    if (equippedWeaponId.empty()) {
        Equip::Ptr wpn = GetItemById("default_sword");
        CC_ASSERT(wpn != nullptr);
        profile.AddOwnedEquip(wpn->id);
        profile.EquipWeapon(wpn);
    }
}

void Store::Buy(const std::string &id)
{
    SessionInfo &session = SessionInfo::Instance();
    
    Equip::Ptr equip = GetItemById(id);
    if (equip && equip->price <= session.GetCoins())
    {
        session.AddCoins(-equip->price);
        session.AddOwnedEquip(equip->id);
        equip->sold = true;
    }
}

Equip::Ptr Store::GetItemById(const std::string &id) const
{
    for (const auto info : _items) {
        if (info->id == id) {
            return info;
        }
    }
    return Equip::Ptr();
}

const Store::Items& Store::GetAllItems() const
{
    return _items;
}

Store::Items Store::GetWeaponItems() const
{
    Items items;
    for (auto item : _items) {
        if (item->type == Equip::Type::WEAPON) {
            items.push_back(item);
        }
    }
    return items;
}

void Store::Reset()
{
    _items.clear();
}
