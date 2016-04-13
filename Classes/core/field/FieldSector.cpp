//
//  FieldSector.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 11/07/15.
//
//

#include "FieldSector.h"
#include "Field.h"
#include "GameInfo.h"
#include "Utils.h"
#include "Log.h"

#include "cocos2d.h"
#include <stdlib.h>
#include <math.h>

const std::string PresetsLibrary::EMPTY_SQUARE = "--";

PresetsLibrary& PresetsLibrary::getInstance()
{
    static PresetsLibrary instance;
    return instance;
}

void PresetsLibrary::load(const std::string &filename)
{
    _presets.clear();
    
    cocos2d::FileUtils *fileUtils = cocos2d::FileUtils::getInstance();
    std::string path = fileUtils->fullPathForFilename(filename);
    std::string buff = fileUtils->getStringFromFile(path);
    
    tinyxml2::XMLDocument document;
    tinyxml2::XMLError result = document.Parse(buff.c_str());
    
    if (result == tinyxml2::XMLError::XML_SUCCESS || result == tinyxml2::XMLError::XML_NO_ERROR)
    {
        auto root = document.RootElement();
        auto node = root->FirstChildElement();
        while (node)
        {
            auto row = node->FirstChildElement();
            
            Preset preset;
            preset.squaresByWidth = calcPresetWidth(row);
            preset.squaresByHeight = calcPresetHeight(row);
            preset.id = node->Attribute("id");
            
            int heightIndex = preset.squaresByHeight - 1;
            
            while (row)
            {
                std::string desc = row->GetText();
                for (int k=0;k<preset.squaresByWidth;k++)
                {
                    std::string group = desc.substr(k*2, 2);
                    if (group != EMPTY_SQUARE)
                    {
                        SpawnSquare square;
                        square.x = k;
                        square.y = heightIndex;
                        square.group = group;
                        preset.spawnMap.push_back(square);
                        preset.obsticlesAmount = 0;
                        
                        /*CHECK*/// This is temporary solution for obstacles cheking
                        const std::string obstacleGroup = "OB";
                        if (group != obstacleGroup)
                        {
                            square.type = Entity::Type::ENEMY;
                            preset.enemiesAmount++;
                        }
                        else
                        {
                            square.type = Entity::Type::OBSTACLE;
                        }
                    }
                }
                row = row->NextSiblingElement();
                heightIndex--;
            }
            
            auto data = std::pair<int, Preset>(preset.enemiesAmount, preset);
            _presets.insert(data);
            
            node = node->NextSiblingElement();
        }
        WRITE_INIT("Presets loaded successfully.");
    }
    else
    {
        WRITE_ERR("Failed to load presets.");
        WRITE_ERR(document.GetErrorStr1());
        WRITE_ERR(document.GetErrorStr2());
    }

}

Preset PresetsLibrary::getRandomWithEnemiesAmount(int amount) const
{
    auto lower = _presets.lower_bound(amount);
    auto upper = _presets.upper_bound(amount);
    
    std::vector<std::pair<int, Preset>> sample;
    std::copy(lower, upper, std::back_inserter(sample));
    
    if (!sample.empty()) {
        int min = 0;
        int max = sample.size() - 1;
        int index = misc::random(min, max);
        return sample[index].second;
    } else {
        return Preset();
    }
}

Preset PresetsLibrary::getPResetById(const std::string &id) const
{
    for (auto it = _presets.cbegin(); it != _presets.end(); it++)
    {
        const Preset &preset = (*it).second;
        if (preset.id == id) {
            return preset;
        }
    }
    return Preset();
}

int PresetsLibrary::calcPresetWidth(tinyxml2::XMLElement *elem) const
{
    int object = 2;
    const char *desc = elem->GetText();
    if (desc) {
        int len = strlen(desc);
        return len / object;
    } else {
        return 0;
    }
}

int PresetsLibrary::calcPresetHeight(tinyxml2::XMLElement *elem) const
{
    int height = 0;
    while (elem) {
        elem = elem->NextSiblingElement();
        height++;
    }
    return height;
}

FieldSector::Ptr FieldSector::create()
{
    return std::make_shared<FieldSector>();
}
 
FieldSector::FieldSector()
: ModelBase()
, _width(0)
, _height(0)
, _logicX(0)
, _logicY(0)
, _squaresByWidth(0)
, _squaresByHeight(0)
{
}

void FieldSector::init(const Preset &preset)
{
    GameInfo &gameinfo = GameInfo::getInstance();
    
    _content = preset.spawnMap;
    _squaresByWidth = preset.squaresByWidth;
    _squaresByHeight = preset.squaresByHeight;
    _squareSize = gameinfo.getConstFloat("SQUARE_SIZE");
    _width = _squaresByWidth * _squareSize;
    _height = _squaresByHeight * _squareSize;
}

void FieldSector::setX(float x)
{
    _logicX = x;
}

void FieldSector::setY(float y)
{
    _logicY = y;
}

float FieldSector::getWidth() const
{
    return _width;
}

float FieldSector::getHeight() const
{
    return _height;
}

float FieldSector::getX() const
{
    return _logicX;
}

float FieldSector::getY() const
{
    return _logicY;
}

int FieldSector::getSquaresNumByX() const
{
    return _squaresByWidth;
}

int FieldSector::getSquaresNumByY() const
{
    return _squaresByHeight;
}

const std::vector<SpawnSquare>& FieldSector::getContent() const
{
    return _content;
}

const std::string& FieldSector::getPresetId() const
{
    return _presetId;
}
