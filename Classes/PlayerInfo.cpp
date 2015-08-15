//
//  PlayerInfo.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 07/06/15.
//
//

#include "PlayerInfo.h"
#include "Store.h"
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
        tinyxml2::XMLNode *root = document.RootElement();
        
        tinyxml2::XMLElement *coins = root->FirstChildElement("Coins");
        _totalCoins = coins->IntAttribute("amount");
        
        tinyxml2::XMLElement *bestScore = root->FirstChildElement("BestScore");
        _bestScore.coins = bestScore->IntAttribute("coins");
        _bestScore.kills = bestScore->IntAttribute("kills");
        
        tinyxml2::XMLElement *ownedEquip = root->FirstChildElement("OwnedEquip")->FirstChildElement();
        while (ownedEquip) {
            std::string equipId = ownedEquip->Attribute("id");
            addOwnedEquip(store.getItemById(equipId));
            ownedEquip = ownedEquip->NextSiblingElement();
        };
        
        tinyxml2::XMLElement *equippedWeapon = root->FirstChildElement("EquippedWeapon");
        _equippedWeaponId = equippedWeapon->Attribute("id");
        
        tinyxml2::XMLElement *equippedArmor = root->FirstChildElement("EquippedArmor");
        _equippedArmorId = equippedArmor->Attribute("id");
        
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
        tinyxml2::XMLDeclaration *declaration = document.NewDeclaration();
        tinyxml2::XMLElement *root = document.NewElement("Save");
        
        tinyxml2::XMLElement *coins = document.NewElement("Coins");
        coins->SetAttribute("amount", cocos2d::StringUtils::toString(_totalCoins).c_str());
        root->LinkEndChild(coins);
        
        tinyxml2::XMLElement *bestScore = document.NewElement("BestScore");
        bestScore->SetAttribute("coins", cocos2d::StringUtils::toString(_bestScore.coins).c_str());
        bestScore->SetAttribute("kills", cocos2d::StringUtils::toString(_bestScore.kills).c_str());
        root->LinkEndChild(bestScore);
        
        tinyxml2::XMLElement *ownedEquip = document.NewElement("OwnedEquip");
        for (const auto &id : _ownedEquips) {
            tinyxml2::XMLElement *equip = document.NewElement("Equip");
            equip->SetAttribute("id", id.c_str());
            ownedEquip->LinkEndChild(equip);
        }
        root->LinkEndChild(ownedEquip);
    
        tinyxml2::XMLElement *equippedWeapon = document.NewElement("EquippedWeapon");
        equippedWeapon->SetAttribute("id", _equippedWeaponId.c_str());
        root->LinkEndChild(equippedWeapon);
        
        tinyxml2::XMLElement *equippedArmor = document.NewElement("EquippedArmor");
        equippedArmor->SetAttribute("id", _equippedArmorId.c_str());
        root->LinkEndChild(equippedArmor);
        
        document.LinkEndChild(declaration);
        document.LinkEndChild(root);
        
        document.SaveFile(file, false);
        fclose(file);
        
        WRITE_LOG("Save completed successfully.");
    }
    else
    {
        CC_ASSERT(false);
    }
}

void PlayerInfo::addOwnedEquip(Equip::Ptr item)
{
    if (!isEquipOwned(item)) {
        _ownedEquips.push_back(item->id);
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
    
    if (item->type == Equip::Type::WEAPON) {
        _equippedWeaponId = item->id;
    } else if (item->type == Equip::Type::ARMOR) {
        _equippedArmorId = item->id;
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
        if (equip == item->id) {
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
    
    if (item->type == Equip::Type::WEAPON) {
        return _equippedWeaponId == item->id;
    } else if (item->type == Equip::Type::ARMOR) {
        return _equippedArmorId == item->id;
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
