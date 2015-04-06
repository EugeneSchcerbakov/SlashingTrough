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
            difficult.sectors = atoi(elem->Attribute("sectors"));
            difficult.obstacles = atoi(elem->Attribute("obstacles"));
            difficult.enemies = atoi(elem->Attribute("enemies"));
            _diffucultSettings.push_back(difficult);
            diffSettingsNode = diffSettingsNode->NextSibling();
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

const GameInfo::DiffucultSettings& GameInfo::GetDiffucultSettings() const
{
    return _diffucultSettings;
}

