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

const std::string PlayerInfo::VarKeyCoins = "Coins";
const std::string PlayerInfo::VarKeyEquipWpn = "EquipedWeapon";
const std::string PlayerInfo::VarKeyEquipArm = "EquipedArmor";

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
        
        auto ownedEquip = root->FirstChildElement("OwnedEquip")->FirstChildElement();
        while (ownedEquip) {
            std::string equipId = ownedEquip->Attribute("id");
            addOwnedEquip(store.getItemById(equipId));
            ownedEquip = ownedEquip->NextSiblingElement();
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
    
    if (variables.getString(VarKeyEquipWpn).empty()) {
        Equip::Ptr weapon = store.getItemById(Store::DEFAULT_WEAPON_ID);
        CC_ASSERT(weapon != nullptr);
        addOwnedEquip(weapon);
        equip(weapon);
    }
    
    if (variables.getString(VarKeyEquipArm).empty()) {
        Equip::Ptr armor = store.getItemById(Store::DEFAULT_ARMOR_ID);
        CC_ASSERT(armor != nullptr);
        addOwnedEquip(armor);
        equip(armor);
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
        
        auto ownedEquip = document.NewElement("OwnedEquip");
        for (const auto &id : _ownedEquips) {
            auto equip = document.NewElement("Equip");
            equip->SetAttribute("id", id.c_str());
            ownedEquip->LinkEndChild(equip);
        }
        root->LinkEndChild(ownedEquip);
        
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

void PlayerInfo::addOwnedEquip(Equip::Ptr item)
{
    if (!isEquipOwned(item)) {
        _ownedEquips.push_back(item->getId());
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

void PlayerInfo::equip(Equip::Ptr item)
{
    if (!item) {
        CC_ASSERT(false);
        return;
    }
    
    CC_ASSERT(isEquipOwned(item));
    
    std::string itemId = item->getId();
    
    if (item->isType(Equip::Type::WEAPON)) {
        variables.setString(VarKeyEquipWpn, itemId);
        WRITE_LOG("Equiped weapon with id: " + itemId);
    } else if (item->isType(Equip::Type::ARMOR)) {
        variables.setString(VarKeyEquipArm, itemId);
        WRITE_LOG("Equiped armor with id: " + itemId);
    } else {
        WRITE_WARN("Failed to equip entity of unknown type.");
        CC_ASSERT(false);
    }
}

int PlayerInfo::getCoins() const
{
    return 0;//_totalCoins;
}

int PlayerInfo::getDamage() const
{
    Equip::Ptr equip = Store::getInstance().getItemById("");
    if (equip) {
        EquipWeapon *weapon = EquipWeapon::cast(equip);
        return floorf(weapon->getDamage());
    } else {
        return 0.0f;
        WRITE_WARN("Failed to cast to weapon.");
    }
}

bool PlayerInfo::isEquipOwned(Equip::Ptr item) const
{
    if (!item) {
        CC_ASSERT(false);
        return false;
    }
    
    for (auto equip : _ownedEquips) {
        if (equip == item->getId()) {
            return true;
        }
    }
    return false;
}

bool PlayerInfo::isEquipped(Equip::Ptr item) const
{
    if (!item) {
        CC_ASSERT(false);
        return false;
    }
    
    if (item->isType(Equip::Type::WEAPON)) {
        return variables.getString(VarKeyEquipWpn) == item->getId();
    } else if (item->isType(Equip::Type::ARMOR)) {
        return variables.getString(VarKeyEquipArm) == item->getId();
    } else {
        CC_ASSERT(false);
    }
}
