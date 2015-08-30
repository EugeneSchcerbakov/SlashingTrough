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

PlayerInfo& PlayerInfo::getInstance()
{
    static PlayerInfo instance;
    return instance;
}

PlayerInfo::PlayerInfo()
: _totalCoins(0)
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
        
        auto coins = root->FirstChildElement("Coins");
        _totalCoins = coins->IntAttribute("amount");
        
        auto bestScore = root->FirstChildElement("BestScore");
        _bestScore.coins = bestScore->IntAttribute("coins");
        _bestScore.kills = bestScore->IntAttribute("kills");
        
        auto ownedEquip = root->FirstChildElement("OwnedEquip")->FirstChildElement();
        while (ownedEquip) {
            std::string equipId = ownedEquip->Attribute("id");
            addOwnedEquip(store.getItemById(equipId));
            ownedEquip = ownedEquip->NextSiblingElement();
        };
        
        auto equippedWeapon = root->FirstChildElement("EquippedWeapon");
        _equippedWeaponId = equippedWeapon->Attribute("id");
        
        auto equippedArmor = root->FirstChildElement("EquippedArmor");
        _equippedArmorId = equippedArmor->Attribute("id");
        
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
    
    if (_equippedWeaponId.empty()) {
        Equip::Ptr weapon = store.getItemById(Store::DEFAULT_WEAPON_ID);
        CC_ASSERT(weapon != nullptr);
        addOwnedEquip(weapon);
        equip(weapon);
    }
    
    if (_equippedArmorId.empty()) {
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
        
        auto coins = document.NewElement("Coins");
        coins->SetAttribute("amount", cocos2d::StringUtils::toString(_totalCoins).c_str());
        root->LinkEndChild(coins);
        
        auto bestScore = document.NewElement("BestScore");
        bestScore->SetAttribute("coins", cocos2d::StringUtils::toString(_bestScore.coins).c_str());
        bestScore->SetAttribute("kills", cocos2d::StringUtils::toString(_bestScore.kills).c_str());
        root->LinkEndChild(bestScore);
        
        auto ownedEquip = document.NewElement("OwnedEquip");
        for (const auto &id : _ownedEquips) {
            auto equip = document.NewElement("Equip");
            equip->SetAttribute("id", id.c_str());
            ownedEquip->LinkEndChild(equip);
        }
        root->LinkEndChild(ownedEquip);
    
        auto equippedWeapon = document.NewElement("EquippedWeapon");
        equippedWeapon->SetAttribute("id", _equippedWeaponId.c_str());
        root->LinkEndChild(equippedWeapon);
        
        auto equippedArmor = document.NewElement("EquippedArmor");
        equippedArmor->SetAttribute("id", _equippedArmorId.c_str());
        root->LinkEndChild(equippedArmor);
        
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
    _totalCoins += coins;
    if (_totalCoins < 0) {
        _totalCoins = 0;
    }
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
        _equippedWeaponId = itemId;
        WRITE_LOG("Equiped weapon with id: " + itemId);
    } else if (item->isType(Equip::Type::ARMOR)) {
        _equippedArmorId = itemId;
        WRITE_LOG("Equiped armor with id: " + itemId);
    } else {
        WRITE_WARN("Failed to equip entity of unknown type.");
        CC_ASSERT(false);
    }
}

void PlayerInfo::setBestScore(const PlayerInfo::Score &score)
{
    _bestScore = score;
}

PlayerInfo::Score PlayerInfo::getBestScore() const
{
    return _bestScore;
}

int PlayerInfo::getCoins() const
{
    return _totalCoins;
}

int PlayerInfo::getDamage() const
{
    Equip::Ptr equip = Store::getInstance().getItemById(getEquippedWeaponId());
    if (equip) {
        EquipWeapon *weapon = EquipWeapon::cast(equip);
        return floorf(weapon->getDamage());
    } else {
        return 0.0f;
        WRITE_WARN("Failed to cast to weapon.");
    }
}

bool PlayerInfo::isBestScore(const PlayerInfo::Score &score) const
{
    return score.coins > _bestScore.coins;
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
        return _equippedWeaponId == item->getId();
    } else if (item->isType(Equip::Type::ARMOR)) {
        return _equippedArmorId == item->getId();
    } else {
        CC_ASSERT(false);
    }
}

const std::string& PlayerInfo::getEquippedWeaponId() const
{
    return _equippedWeaponId;
}

const std::string& PlayerInfo::getEquippeArmorId() const
{
    return _equippedArmorId;
}
