//
//  GameInfo.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 01/04/15.
//
//

#include "GameInfo.h"
#include "Log.h"
#include "cocos2d.h"
#include "tinyxml2/tinyxml2.h"

GameInfo& GameInfo::getInstance()
{
    static GameInfo gameInfo;
    return gameInfo;
}

bool GameInfo::loadInfo(const std::string &filename)
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
                _constants.setInt(name, atoi(value.c_str()));
            } else if (type == "bool") {
                _constants.setBool(name, value == "true");
            } else if (type == "float") {
                _constants.setFloat(name, atof(value.c_str()));
            } else if (type == "string") {
                _constants.setString(name, value);
            } else {
                assert(false);
            }
            globVarNode = globVarNode->NextSibling();
        }
        tinyxml2::XMLNode *objects = globVar->NextSibling();
        tinyxml2::XMLNode *objectsNode = objects->FirstChild();
        while (objectsNode) {
            tinyxml2::XMLElement *elem = objectsNode->ToElement();
            std::string type = elem->Name();
            if (type == "Obstacle") {
                ObstacleType info;
                info.destructible = elem->BoolAttribute("destructible");
                info.health = elem->FloatAttribute("health");
                info.damage = elem->FloatAttribute("damage");
                info.staminaDrainPoints = elem->FloatAttribute("staminaDrainPoints");
                info.killPoints = elem->IntAttribute("killPoints");
                info.goldPointsReward = elem->IntAttribute("goldPointsReward");
                info.staminaPointsReward = elem->IntAttribute("staminaPointsReward");
                info.scorePointsReward = elem->IntAttribute("scorePointsReward");
                info.sprite = elem->Attribute("sprite");
                std::string name = elem->Attribute("name");
                _obstaclesSettings[name] = info;
            } else if (type == "Enemy") {
                auto rangeAttackInfo = elem->FirstChildElement("RangeAttack");
                auto melleAttackInfo = elem->FirstChildElement("MelleAttack");
                EnemyType info;
                info.damage = elem->FloatAttribute("damage");
                info.health = elem->FloatAttribute("health");
                info.staminaDrainPoints = elem->FloatAttribute("staminaDrainPoints");
                info.killPoints = elem->IntAttribute("killPoints");
                info.goldPointsReward = elem->IntAttribute("goldPointsReward");
                info.staminaPointsReward = elem->IntAttribute("staminaPointsReward");
                info.scorePointsReward = elem->IntAttribute("scorePointsReward");
                info.sprite = elem->Attribute("sprite");
                info.model = elem->Attribute("model") ? elem->Attribute("model") : "";
                info.colorR = elem->IntAttribute("R");
                info.colorG = elem->IntAttribute("G");
                info.colorB = elem->IntAttribute("B");
                std::string name = elem->Attribute("name");
                if (rangeAttackInfo) {
                    info.rangeAttack.distance = rangeAttackInfo->FloatAttribute("distance");
                    info.rangeAttack.recoveryTime = rangeAttackInfo->FloatAttribute("recoveryTime");
                    info.rangeAttack.projectile.healthDamage = rangeAttackInfo->FloatAttribute("healthDamage");
                    info.rangeAttack.projectile.speed = rangeAttackInfo->FloatAttribute("projectileSpeed");
                    info.rangeAttack.projectile.lifeTime = rangeAttackInfo->FloatAttribute("projectileLifeTime");
                    info.rangeAttack.projectile.texture = rangeAttackInfo->Attribute("projectileTexture");
                    info.rangeAttack.allowed = true;
                }
                if (melleAttackInfo) {
                    std::string condition = melleAttackInfo->Attribute("condition");
                    if (condition == "same_side") {
                        info.melleAttack.condition = EnemyType::Melle::SAME_LINE;
                    } else if (condition == "left_side") {
                        info.melleAttack.condition = EnemyType::Melle::LEFT_LINE;
                    } else if (condition == "right_side") {
                        info.melleAttack.condition = EnemyType::Melle::RIGHT_LINE;
                    } else {
                        CC_ASSERT(false);
                    }
                    info.melleAttack.areaWidth = melleAttackInfo->FloatAttribute("areaWidth");
                    info.melleAttack.areaHeight = melleAttackInfo->FloatAttribute("areaHeight");
                    info.melleAttack.dmgHealth = melleAttackInfo->FloatAttribute("dmgHealth");
                    info.melleAttack.showHighlightRange = melleAttackInfo->FloatAttribute("showHighlightRange");
                    info.melleAttack.showHighlightTime = melleAttackInfo->FloatAttribute("showHighlightTime");
                    info.melleAttack.recoveryTime = melleAttackInfo->FloatAttribute("recoveryTime");
                    info.melleAttack.mustShowHighlight = melleAttackInfo->BoolAttribute("mustShowHighlight");
                    info.melleAttack.allowed = true;
                }
                _enemiesSettings[name] = info;
            } else {
                WRITE_WARN("Unknown entity in GameInfo");
                CC_ASSERT(false);
            }
            objectsNode = objectsNode->NextSibling();
        }
        WRITE_INIT("GameInfo successfully created.");
        return true;
    } else {
        WRITE_ERR("Failed to read GameInfo");
        return false;
    }
}

int GameInfo::getConstInt(const std::string &name, int def) const
{
    return _constants.getInt(name, def);
}

bool GameInfo::getConstBool(const std::string &name, bool def) const
{
    return _constants.getBool(name, def);
}

float GameInfo::getConstFloat(const std::string &name, float def) const
{
    return _constants.getFloat(name, def);
}

std::string GameInfo::getConstString(const std::string &name, std::string def) const
{
    return _constants.getString(name, def);
}

int GameInfo::getGlobalInt(const std::string &name, int def) const
{
    return _globals.getInt(name, def);
}

bool GameInfo::getGlobalBool(const std::string &name, bool def) const
{
    return _globals.getBool(name, def);
}

float GameInfo::getGlobalFloat(const std::string &name, float def) const
{
    return _globals.getFloat(name, def);
}

std::string GameInfo::getGlobalString(const std::string &name, std::string def) const
{
    return _globals.getString(name, def);
}

void GameInfo::setGlobalInt(const std::string &name, int value)
{
    _globals.setInt(name, value);
}

void GameInfo::setGlobalBool(const std::string &name, bool value)
{
    _globals.setBool(name, value);
}

void GameInfo::setGlobalFloat(const std::string &name, float value)
{
    _globals.setFloat(name, value);
}

void GameInfo::setGlobalString(const std::string &name, const std::string &value)
{
    _globals.setString(name, value);
}

Entity::Type GameInfo::getTypeById(const std::string &id) const
{
    auto oit = _obstaclesSettings.find(id);
    if (oit != _obstaclesSettings.end()) {
        return Entity::Type::OBSTACLE;
    }
    auto eit = _enemiesSettings.find(id);
    if (eit != _enemiesSettings.end()) {
        return Entity::Type::ENEMY;
    }
    
    return Entity::Type::NONE;
}

const GameInfo::ObstacleType& GameInfo::getObstacleInfoByName(const std::string &name) const
{
    ObstaclesSettings::const_iterator it;
    it = _obstaclesSettings.find(name);
    if (it != _obstaclesSettings.end()) {
        return (*it).second;
    } else {
        WRITE_WARN("Failed to get entity: " + name);
        return (*_obstaclesSettings.end()).second;
    }
}

const GameInfo::EnemyType& GameInfo::getEnemyInfoByName(const std::string &name) const
{
    EnemiesSettings::const_iterator it;
    it = _enemiesSettings.find(name);
    if (it != _enemiesSettings.end()) {
        return (*it).second;
    } else {
        WRITE_WARN("Failed to get enemy: " + name);
        return (*_enemiesSettings.end()).second;
    }
}

GameInfo::GameplayObjectsTypes GameInfo::getObstaclesTypes() const
{
    GameplayObjectsTypes types;
    types.reserve(_obstaclesSettings.size());
    for (auto object : _obstaclesSettings) {
        types.push_back(object.first);
    }
    return types;
}

GameInfo::GameplayObjectsTypes GameInfo::getEnemiesTypes() const
{
    GameplayObjectsTypes types;
    types.reserve(_obstaclesSettings.size());
    for (auto object : _enemiesSettings) {
        types.push_back(object.first);
    }
    return types;
}


