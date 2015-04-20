//
//  GameInfo.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 01/04/15.
//
//

#include "GameInfo.h"
#include "external/tinyxml2/tinyxml2.h"

GameInfo& GameInfo::Instance()
{
    static GameInfo gameInfo;
    return gameInfo;
}

bool GameInfo::LoadInfo(const std::string &filename)
{
    cocos2d::FileUtils *fileUtils = cocos2d::FileUtils::getInstance();
    std::string path = fileUtils->fullPathForFilename(filename);
    std::string buff = fileUtils->getStringFromFile(path);
    
    tinyxml2::XMLDocument document;
    tinyxml2::XMLError result = document.Parse(buff.c_str());
    
    if (result == tinyxml2::XMLError::XML_SUCCESS || result == tinyxml2::XMLError::XML_NO_ERROR)
    {
        tinyxml2::XMLNode *root = document.RootElement();
        tinyxml2::XMLNode *globVar = root->FirstChild();
        tinyxml2::XMLNode *globVarNode = globVar->FirstChild();
        while (globVarNode) {
            tinyxml2::XMLElement *elem = globVarNode->ToElement();
            std::string name = elem->Attribute("name");
            std::string type = elem->Attribute("type");
            std::string value = elem->Attribute("value");
            if (type == "int") {
                SetInt(name, atoi(value.c_str()));
            } else if (type == "bool") {
                SetBool(name, (bool)atoi(value.c_str()));
            } else if (type == "float") {
                SetFloat(name, atof(value.c_str()));
            } else if (type == "string") {
                SetString(name, value);
            } else {
                assert(false);
            }
            globVarNode = globVarNode->NextSibling();
        }
        tinyxml2::XMLNode *diffSettings = globVar->NextSibling();
        tinyxml2::XMLNode *diffSettingsNode = diffSettings->FirstChild();
        _diffucultSettings.clear();
        while (diffSettingsNode) {
            tinyxml2::XMLElement *elem = diffSettingsNode->ToElement();
            GameInfo::DiffucultInfo difficult;
            difficult.sectors = elem->IntAttribute("sectors");
            difficult.obstacles = elem->IntAttribute("obstacles");
            difficult.enemies = elem->IntAttribute("enemies");
            _diffucultSettings.push_back(difficult);
            diffSettingsNode = diffSettingsNode->NextSibling();
        }
        tinyxml2::XMLNode *objects = diffSettings->NextSibling();
        tinyxml2::XMLNode *objectsNode = objects->FirstChild();
        while (objectsNode) {
            tinyxml2::XMLElement *elem = objectsNode->ToElement();
            std::string type = elem->Name();
            if (type == "Obstacle") {
                bool destructible = elem->BoolAttribute("destructible");
                float health = elem->FloatAttribute("health");
                float damage = elem->FloatAttribute("damage");
                std::string sprite = elem->Attribute("sprite");
                std::string name = elem->Attribute("name");
                _obstaclesSettings[name] = ObstacleType(health, damage, destructible, sprite);
            } else if (type == "Enemy") {
                float damage = elem->FloatAttribute("damage");
                float health = elem->FloatAttribute("health");
                std::string sprite = elem->Attribute("sprite");
                std::string name = elem->Attribute("name");
                _enemiesSettings[name] = EnemyType(damage, health, sprite);
            } else {
                CC_ASSERT(false);
            }
            objectsNode = objectsNode->NextSibling();
        }
        return true;
    } else {
        return false;
    }
}

int GameInfo::GetInt(const std::string &name, int def) const
{
    std::map<std::string, int>::const_iterator it;
    it = _variablesInt.find(name);
    return it != _variablesInt.end() ? (*it).second : def;
}

bool GameInfo::GetBool(const std::string &name, bool def) const
{
    return GetInt(name, (int)def) > 0;
}

float GameInfo::GetFloat(const std::string &name, float def) const
{
    std::map<std::string, float>::const_iterator it;
    it = _variablesFloat.find(name);
    return it != _variablesFloat.end() ? (*it).second : def;
}

std::string GameInfo::GetString(const std::string &name, std::string def) const
{
    std::map<std::string, std::string>::const_iterator it;
    it = _variablesStr.find(name);
    return it != _variablesStr.end() ? (*it).second : def;
}

void GameInfo::SetInt(const std::string &name, int value)
{
    _variablesInt[name] = value;
}

void GameInfo::SetBool(const std::string &name, bool value)
{
    SetInt(name, (int)value);
}

void GameInfo::SetFloat(const std::string &name, float value)
{
    _variablesFloat[name] = value;
}

void GameInfo::SetString(const std::string &name, const std::string &value)
{
    _variablesStr[name] = value;
}

const GameInfo::ObstacleType& GameInfo::GetObstacleInfoByName(const std::string &name) const
{
    ObstaclesSettings::const_iterator it;
    it = _obstaclesSettings.find(name);
    if (it != _obstaclesSettings.end()) {
        return (*it).second;
    } else {
        CC_ASSERT(false);
        return (*_obstaclesSettings.end()).second;
    }
}

const GameInfo::EnemyType& GameInfo::GetEnemyInfoByName(const std::string &name) const
{
    EnemiesSettings::const_iterator it;
    it = _enemiesSettings.find(name);
    if (it != _enemiesSettings.end()) {
        return (*it).second;
    } else {
        CC_ASSERT(false);
        return (*_enemiesSettings.end()).second;
    }
}

GameInfo::GameplayObjectsTypes GameInfo::GetObstaclesTypes() const
{
    GameplayObjectsTypes types;
    types.reserve(_obstaclesSettings.size());
    for (auto object : _obstaclesSettings) {
        types.push_back(object.first);
    }
    return types;
}

GameInfo::GameplayObjectsTypes GameInfo::GetEnemiesTypes() const
{
    GameplayObjectsTypes types;
    types.reserve(_obstaclesSettings.size());
    for (auto object : _enemiesSettings) {
        types.push_back(object.first);
    }
    return types;
}

const GameInfo::DiffucultSettings& GameInfo::GetDiffucultSettings() const
{
    return _diffucultSettings;
}
