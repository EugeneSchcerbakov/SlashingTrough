//
//  PlayerInfo.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 07/06/15.
//
//

#include "PlayerInfo.h"
#include "Store.h"
#include "LevelsCache.h"
#include "Log.h"

#include "cocos2d.h"
#include "tinyxml2/tinyxml2.h"
#include <stdio.h>

// PlayerInventory

void PlayerInventory::add(Item::WeakPtr item, int amount)
{
    if (item.expired()) {
        WRITE_WARN("Failed to add invalid item.");
    }
    
    Item::Ptr pointer = item.lock();
    auto it = _items.find(pointer->getId());
    if (it != _items.end()) {
        (*it).second.amount += amount;
    } else{
        PlayerItem newItem;
        newItem.item = pointer;
        newItem.amount = amount;
        
        auto pair = std::pair<std::string, PlayerItem>(pointer->getId(), newItem);
        _items.insert(pair);
    }
}

void PlayerInventory::remove(Item::WeakPtr item)
{
    if (item.expired()) {
        WRITE_WARN("Failed to remove invalid item.");
    }
    
    Item::Ptr pointer = item.lock();
    auto it = _items.find(pointer->getId());
    if (it != _items.end()) {
        _items.erase(it);
    } else {
        WRITE_WARN("Trying to remove unexisted item.");
    }
}

int PlayerInventory::getAmount(const std::string &id) const
{
    auto it = _items.find(id);
    return it != _items.end() ? (*it).second.amount : 0;
}

int PlayerInventory::getAmount(Item::WeakPtr item) const
{
    if (item.expired()) {
        WRITE_WARN("Failed to get amount if invalid item.");
        return 0;
    }
    
    Item::Ptr pointer = item.lock();
    return getAmount(pointer->getId());
}

bool PlayerInventory::owned(const std::string &id) const
{
    return getAmount(id) > 0;
}

bool PlayerInventory::owned(Item::WeakPtr item) const
{
    return getAmount(item) > 0;
}

PlayerInventory::PlayerItem& PlayerInventory::operator[](const std::string &id)
{
    auto it = _items.find(id);
    return it != _items.end() ? (*it).second : (*_items.begin()).second;
}

// PlayerInfo

const std::string PlayerInfo::VarKeyCoins = "Coins";
const std::string PlayerInfo::VarKeyItemWpn = "EquipedWeapon";
const std::string PlayerInfo::VarKeyItemArm = "EquipedArmor";

const std::string PlayerInfo::DEFAULT_WEAPON_ID = "default_sword";
const std::string PlayerInfo::DEFAULT_ARMOR_ID = "default_armor";

PlayerInfo& PlayerInfo::getInstance()
{
    static PlayerInfo instance;
    return instance;
}

PlayerInfo::PlayerInfo()
{
}

void PlayerInfo::load(const std::string &filename)
{
    _saveFileName = filename;
    
	std::string path, fullPath;

	auto fileUtils = cocos2d::FileUtils::getInstance();
	auto platform = cocos2d::Application::getInstance()->getTargetPlatform();
	if (platform == cocos2d::Application::Platform::OS_WINDOWS) {
		path = "";
	} else {
		path = fileUtils->getWritablePath();
	}
	fullPath = path + _saveFileName;

    if (!fileUtils->isFileExist(fullPath))
    {
        WRITE_INIT("Default save file created.");
        save();
    }
    
    std::string buffer = fileUtils->getStringFromFile(fullPath);
    
    tinyxml2::XMLDocument document;
    tinyxml2::XMLError result = document.Parse(buffer.c_str());
    
    Store &store = Store::getInstance();
    
    if (result == tinyxml2::XMLError::XML_SUCCESS || result == tinyxml2::XMLError::XML_NO_ERROR)
    {
        auto root = document.RootElement();
        
        auto vars = root->FirstChildElement("Variables");
        auto elem = vars->FirstChildElement();
        while (elem) {
            std::string name = elem->Attribute("name");
            std::string type = elem->Attribute("type");
            std::string data = elem->Attribute("data");
            if (type == "int") {
                variables.setInt(name, atoi(data.c_str()));
            } else if (type == "float") {
                variables.setFloat(name, atof(data.c_str()));
            } else if (type == "string") {
                variables.setString(name, data);
            } else {
                WRITE_WARN("Unknown save file data leem type:" + type);
            }
            elem = elem->NextSiblingElement();
        }
        
        auto inventory = root->FirstChildElement("Inventory");
        auto equip = inventory->FirstChildElement();
        while (equip) {
            std::string id = equip->Attribute("id");
            int amount = equip->IntAttribute("amount");
            Inventory.add(store.getItemById(id), amount);
            equip = equip->NextSiblingElement();
        };
        
        LevelsCache &levelsCache = LevelsCache::getInstance();
        
        auto levelsProgress = root->FirstChildElement("LevelsProgress");
        auto levelElem = levelsProgress->FirstChildElement();
        while (levelElem)
        {
            std::string levelId = levelElem->Attribute("id");
            
            FieldLevel::SaveData data;
            data.status = FieldLevel::stringToStatus(levelElem->Attribute("status"));
            
            auto drops = levelElem->FirstChildElement("OccurredDrops");
            auto dropElem = drops->FirstChildElement();
            while (dropElem)
            {
                FieldLevel::SaveData::DropFact fact;
                fact.first = dropElem->Attribute("itemId");
                fact.second = dropElem->BoolAttribute("dropped");
                data.occurredDrop.push_back(fact);
                dropElem = dropElem->NextSiblingElement();
            }
            
            FieldLevel::WeakPtr level_ptr = levelsCache.getLevelById(levelId);
            if (!level_ptr.expired()) {
                FieldLevel::Ptr level = level_ptr.lock();
                level->restore(data);
            } else {
                WRITE_WARN("Failed to restore level with id: " + levelId);
            }
            
            levelElem = levelElem->NextSiblingElement();
        }
        
        WRITE_INIT("Save file successfully loaded.");
    }
    else
    {
        WRITE_ERR("Failed to read save file.");
    }
    
    if (Inventory.getAmount(PlayerInfo::DEFAULT_WEAPON_ID) <= 0) {
        Inventory.add(store.getItemById(PlayerInfo::DEFAULT_WEAPON_ID));
    }
    if (Inventory.getAmount(PlayerInfo::DEFAULT_ARMOR_ID) <= 0) {
        Inventory.add(store.getItemById(PlayerInfo::DEFAULT_ARMOR_ID));
    }
    
    if (variables.getString(VarKeyItemWpn).empty()) {
        equip(Inventory[PlayerInfo::DEFAULT_WEAPON_ID].item);
    }
    
    if (variables.getString(VarKeyItemArm).empty()) {
        equip(Inventory[PlayerInfo::DEFAULT_ARMOR_ID].item);
    }
}

void PlayerInfo::save()
{
	std::string path, fullPath;

    auto fileUtils = cocos2d::FileUtils::getInstance();
	auto platform = cocos2d::Application::getInstance()->getTargetPlatform();
	if (platform == cocos2d::Application::Platform::OS_WINDOWS) {
		path = ""; // save in the same folder with exe if running under windows
	} else {
		path = fileUtils->getWritablePath();
	}

	fullPath = path + _saveFileName;
    
	FILE *file = fopen(fullPath.c_str(), "w");
    if (file)
    {
        tinyxml2::XMLDocument document;
        auto declaration = document.NewDeclaration();
        auto root = document.NewElement("Save");
        
        auto vars = document.NewElement("Variables");
        for (auto pair : variables._variablesInt) {
            auto data = document.NewElement("DataElem");
            data->SetAttribute("name", pair.first.c_str());
            data->SetAttribute("type", "int");
            data->SetAttribute("data", pair.second);
            vars->LinkEndChild(data);
        }
        for (auto pair : variables._variablesFloat) {
            auto data = document.NewElement("DataElem");
            data->SetAttribute("name", pair.first.c_str());
            data->SetAttribute("type", "float");
            data->SetAttribute("data", pair.second);
            vars->LinkEndChild(data);
        }
        for (auto pair : variables._variablesStr) {
            auto data = document.NewElement("DataElem");
            data->SetAttribute("name", pair.first.c_str());
            data->SetAttribute("type", "string");
            data->SetAttribute("data", pair.second.c_str());
            vars->LinkEndChild(data);
        }
        root->LinkEndChild(vars);
        
        auto inventory = document.NewElement("Inventory");
        for (const auto &pair : Inventory._items) {
            auto equip = document.NewElement("Item");
            equip->SetAttribute("id", pair.first.c_str());
            equip->SetAttribute("amount", pair.second.amount);
            inventory->LinkEndChild(equip);
        }
        root->LinkEndChild(inventory);
    
        LevelsCache &levelsCache = LevelsCache::getInstance();
        auto levelsProgress = document.NewElement("LevelsProgress");
        for (int k = 0; k < levelsCache.getLevelsAmount(); k++)
        {
            FieldLevel::WeakPtr level_ptr = levelsCache.getLevelByIndex(k);
            if (!level_ptr.expired())
            {
                FieldLevel::Ptr level = level_ptr.lock();
                FieldLevel::SaveData data = level->getSaveData();
                
                auto node = document.NewElement("Level");
                node->SetAttribute("id", level->getId().c_str());
                node->SetAttribute("status", FieldLevel::statusToString(data.status).c_str());
                
                auto dropNode = document.NewElement("OccurredDrops");
                for (auto drop : data.occurredDrop) {
                    auto child = document.NewElement("Drop");
                    child->SetAttribute("itemId", drop.first.c_str());
                    child->SetAttribute("dropped", drop.second);
                    dropNode->LinkEndChild(child);
                }
                
                node->LinkEndChild(dropNode);
                
                levelsProgress->LinkEndChild(node);
            }
        }
        
        root->LinkEndChild(levelsProgress);
        
        document.LinkEndChild(declaration);
        document.LinkEndChild(root);
        
        document.SaveFile(file, false);
        fclose(file);
        
        WRITE_INIT("Save completed successfully.");
    }
    else
    {
        CC_ASSERT(false);
    }
}

void PlayerInfo::addCoins(int coins)
{
    int total = variables.getInt(VarKeyCoins);
    total = total + coins;
    if (total < 0) {
        total = 0;
        WRITE_WARN("Negative coins amount");
    }
    variables.setInt(VarKeyCoins, total);
}

void PlayerInfo::equip(Item::Ptr item)
{
    if (!item) {
        CC_ASSERT(false);
        return;
    }
    
    CC_ASSERT(Inventory.owned(item));
    
    std::string itemId = item->getId();
    
    if (item->isType(Item::Type::WEAPON)) {
        variables.setString(VarKeyItemWpn, itemId);
        WRITE_LOG("Equiped weapon with id: " + itemId);
    } else if (item->isType(Item::Type::ARMOR)) {
        variables.setString(VarKeyItemArm, itemId);
        WRITE_LOG("Equiped armor with id: " + itemId);
    } else {
        WRITE_WARN("Failed to equip entity of unknown type.");
        CC_ASSERT(false);
    }
}

int PlayerInfo::getCoins() const
{
    return variables.getInt(VarKeyCoins);
}

int PlayerInfo::getDamage() const
{
    Item::Ptr equip = Store::getInstance().getItemById("");
    if (equip) {
        ItemWeapon *weapon = ItemWeapon::cast(equip);
        return floorf(weapon->getDamage());
    } else {
        return 0.0f;
        WRITE_WARN("Failed to cast to weapon.");
    }
}

bool PlayerInfo::equipped(Item::Ptr item) const
{
    if (!item) {
        CC_ASSERT(false);
        return false;
    }
    
    if (item->isType(Item::Type::WEAPON)) {
        return variables.getString(VarKeyItemWpn) == item->getId();
    } else if (item->isType(Item::Type::ARMOR)) {
        return variables.getString(VarKeyItemArm) == item->getId();
    } else {
        CC_ASSERT(false);
		return false;
    }
}
