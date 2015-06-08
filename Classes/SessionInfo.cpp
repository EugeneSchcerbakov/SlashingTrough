//
//  SessionInfo.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 07/06/15.
//
//

#include "SessionInfo.h"
#include "cocos2d.h"
#include "tinyxml2/tinyxml2.h"

#include <stdio.h>

SessionInfo& SessionInfo::Instance()
{
    static SessionInfo instance;
    return instance;
}

SessionInfo::SessionInfo()
: _totalCoins(0)
{
}

void SessionInfo::Load(const std::string &filename)
{
    _saveFileName = filename;
    
    auto fileUtils = cocos2d::FileUtils::getInstance();
    std::string path = fileUtils->getWritablePath();
    std::string fullPath = path + _saveFileName;
    
    if (!fileUtils->isFileExist(fullPath))
    {
        Save();
    }
    
    std::string buffer = fileUtils->getStringFromFile(fullPath);
    
    tinyxml2::XMLDocument document;
    tinyxml2::XMLError result = document.Parse(buffer.c_str());
    
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
            AddOwnedEquip(equipId);
            ownedEquip = ownedEquip->NextSiblingElement();
        };
        
        tinyxml2::XMLElement *equippedWeapon = root->FirstChildElement("EquippedWeapon");
        _equippedWeaponId = equippedWeapon->Attribute("id");
    }
}

void SessionInfo::Save()
{
    auto fileUtils = cocos2d::FileUtils::getInstance();
    std::string path = fileUtils->getWritablePath();
    std::string fullPath = path + _saveFileName;
    
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
        
        document.LinkEndChild(declaration);
        document.LinkEndChild(root);
        
        document.SaveFile(file, false);
        fclose(file);
    }
    else
    {
        CC_ASSERT(false);
    }
}

void SessionInfo::AddOwnedEquip(const std::string id)
{
    if (!IsEquipOwned(id)) {
        _ownedEquips.push_back(id);
    }
}

void SessionInfo::AddCoins(int coins)
{
    _totalCoins += coins;
    if (_totalCoins < 0) {
        _totalCoins = 0;
    }
}

void SessionInfo::EquipWeapon(Equip::WeakPtr weapon)
{
    if (weapon.expired()) {
        CC_ASSERT(false);
        return;
    }
    
    class EquipWeapon *ptr = EquipWeapon::cast(weapon.lock());
    CC_ASSERT(IsEquipOwned(ptr->id));

    _equippedWeaponId = ptr->id;
}

void SessionInfo::SetBestScore(const SessionInfo::Score &score)
{
    _bestScore = score;
}

SessionInfo::Score SessionInfo::GetBestScore() const
{
    return _bestScore;
}

int SessionInfo::GetCoins() const
{
    return _totalCoins;
}

bool SessionInfo::IsBestScore(const SessionInfo::Score &score) const
{
    return score.coins > _bestScore.coins;
}

bool SessionInfo::IsEquipOwned(const std::string &id) const
{
    for (auto equip : _ownedEquips) {
        if (equip == id) {
            return true;
        }
    }
    return false;
}

bool SessionInfo::IsWeaponEquipped(const std::string &id) const
{
    return _equippedWeaponId == id;
}

const std::string& SessionInfo::GetEquippedWeaponId() const
{
    return _equippedWeaponId;
}

