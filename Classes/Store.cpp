//
//  Store.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 01/06/15.
//
//

#include "Store.h"

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
                Store::WeaponItem weapon;
                weapon.id = elem->Attribute("id");
                weapon.price = elem->IntAttribute("price");
                weapon.damage = elem->FloatAttribute("damage");
                weapon.speed = elem->FloatAttribute("speed");
                weapon.desc = elem->Attribute("desc");
                weapon.icon = elem->Attribute("icon");
                weapon.name = elem->Attribute("name");
                _items.push_back(weapon);
            } else {
                CC_ASSERT(false);
            }
            node = node->NextSibling();
        }
    }
}

Store::Item Store::GetItemById(const std::string &id) const
{
    for (const Item &info : _items) {
        if (info.id == id) {
            return info;
        }
    }
    return Item();
}

const Store::Items& Store::GetAllItems() const
{
    return _items;
}

void Store::Reset()
{
    _items.clear();
}
