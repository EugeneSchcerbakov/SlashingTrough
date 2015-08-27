//
//  LevelsCache.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 24/08/15.
//
//

#include "LevelsCache.h"

#include "Log.h"
#include "cocos2d.h"
#include "tinyxml2/tinyxml2.h"

LevelsCache& LevelsCache::getInstance()
{
    static LevelsCache instance;
    return instance;
}

bool LevelsCache::load(const std::string &filename)
{
    cocos2d::FileUtils *fileUtils = cocos2d::FileUtils::getInstance();
    std::string path = fileUtils->fullPathForFilename(filename);
    std::string buff = fileUtils->getStringFromFile(path);
    
    tinyxml2::XMLDocument document;
    tinyxml2::XMLError result = document.Parse(buff.c_str());
    
    if (result == tinyxml2::XMLError::XML_SUCCESS || result == tinyxml2::XMLError::XML_NO_ERROR)
    {
        auto root = document.RootElement();
        auto node = root->FirstChild();
        while (node) {
            std::string id = node->ToElement()->Attribute("id");
            
            FieldLevel::Ptr level = FieldLevel::create();
            level->initFromXml(node);
            
            auto pair = std::pair<std::string, FieldLevel::Ptr>(id, level);
            _levels.insert(pair);
        
            node = node->NextSibling();
        }
        
        WRITE_INIT("Levels cache successfully created.");
        return true;
    }
    else
    {
        WRITE_ERR("Failed to read levels");
        return false;
    }
}

FieldLevel::WeakPtr LevelsCache::getLevelById(const std::string &id)
{
    auto it = _levels.find(id);
    return it != _levels.end() ? (*it).second : FieldLevel::Ptr();
}

FieldLevel::WeakPtr LevelsCache::getLevelByIndex(int index)
{
    if (index > 0 && index < getLevelsAmount()) {
        int current = 0;
        for (auto it = _levels.begin(); it != _levels.end(); ++it) {
            if (current == index) {
                return (*it).second;
            } else {
                current++;
            }
        }
    }
    return FieldLevel::Ptr();
}

int LevelsCache::getLevelsAmount() const
{
    return _levels.size();
}
