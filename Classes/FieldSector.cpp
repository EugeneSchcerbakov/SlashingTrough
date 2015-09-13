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

const std::string PresetsLoader::EMPTY_SQUARE = "--";

PresetsLoader& PresetsLoader::getInstance()
{
    static PresetsLoader instance;
    return instance;
}

void PresetsLoader::load(const std::string &filename)
{
    GameInfo &gameinfo = GameInfo::getInstance();
    
    cocos2d::FileUtils *fileUtils = cocos2d::FileUtils::getInstance();
    std::string path = fileUtils->fullPathForFilename(filename);
    std::string buff = fileUtils->getStringFromFile(path);
    
    tinyxml2::XMLDocument document;
    tinyxml2::XMLError result = document.Parse(buff.c_str());

    if (result == tinyxml2::XMLError::XML_SUCCESS || result == tinyxml2::XMLError::XML_NO_ERROR)
    {
        auto root = document.RootElement();
        auto node = root->FirstChildElement();
        while (node) {
            auto row = node->FirstChildElement();
            bool is_link = strcmp(node->Name(), "Link") == 0;
            Preset preset;
            preset.squaresByWidth = calcPresetWidth(row);
            preset.squaresByHeight = calcPresetHeight(row);
            preset.difficult = node->Attribute("dif") ? node->IntAttribute("dif") : 0;
            preset.id = node->Attribute("id");
            int heightIndex = preset.squaresByHeight - 1;
            while (row) {
                std::string desc = row->GetText();
                for (int k=0;k<preset.squaresByWidth;k++) {
                    std::string name = desc.substr(k*2, 2);
                    if (name != EMPTY_SQUARE) {
                        Entity::Type type = gameinfo.getTypeById(name);
                        if (type != Entity::Type::NONE) {
                            Square square;
                            square.x = k;
                            square.y = heightIndex;
                            square.type = type;
                            square.name = name;
                            preset.map.push_back(square);
                        }
                    }
                }
                row = row->NextSiblingElement();
                heightIndex--;
            }
            node = node->NextSiblingElement();
            if (!is_link) {
                auto data = std::pair<int, Preset>(preset.difficult, preset);
                _presets.insert(data);
            } else {
                _links.push_back(preset);
            }
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

int PresetsLoader::calcPresetWidth(tinyxml2::XMLElement *elem) const
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

int PresetsLoader::calcPresetHeight(tinyxml2::XMLElement *elem) const
{
    int height = 0;
    while (elem) {
        elem = elem->NextSiblingElement();
        height++;
    }
    return height;
}

Preset PresetsLoader::getPresetById(const std::string &id) const
{
    for (auto it = _presets.begin(); it != _presets.end(); ++it) {
        const auto &preset = (*it).second;
        if (preset.id == id) {
            return preset;
        }
    }
    return (*_presets.begin()).second;
}

Preset PresetsLoader::getRandomPresetWithDif(int difficut) const
{
    auto lower = _presets.lower_bound(difficut);
    auto upper = _presets.upper_bound(difficut);
    
    std::vector<std::pair<int, Preset>> sample;
    std::copy(lower, upper, std::back_inserter(sample));
    
    if (!sample.empty()) {
        int min = 0;
        int max = sample.size() - 1;
        int index = misc::random(min, max);
        Preset preset = sample[index].second;
        return preset;
    } else {
        return Preset();
    }
}

Preset PresetsLoader::getLinkById(const std::string &id) const
{
    for (auto it = _links.begin(); it != _links.end(); ++it) {
        const auto &preset = (*it);
        if (preset.id == id) {
            return preset;
        }
    }
    return (*_links.begin());
}

Preset PresetsLoader::getRandomLink() const
{
    int min = 0;
    int max = _links.size() - 1;
    int index = misc::random(min, max);
    return _links[index];
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
, _runningSpeed(0.0f)
, _squaresByWidth(0)
, _squaresByHeight(0)
{
}

void FieldSector::init(const Preset &preset, float runningSpeed)
{
    GameInfo &gameinfo = GameInfo::getInstance();
    
    _content = preset.map;
    _squaresByWidth = preset.squaresByWidth;
    _squaresByHeight = preset.squaresByHeight;
    _squareSize = gameinfo.getConstFloat("SQUARE_SIZE");
    _width = _squaresByWidth * _squareSize;
    _height = _squaresByHeight * _squareSize;
    _runningSpeed = runningSpeed;
    _presetId = preset.id;
}

void FieldSector::setX(float x)
{
    _logicX = x;
}

void FieldSector::setY(float y)
{
    _logicY = y;
}

float FieldSector::getRunningSpeed() const
{
    return _runningSpeed;
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

const std::vector<Square>& FieldSector::getContent() const
{
    return _content;
}

const std::string& FieldSector::getPresetId() const
{
    return _presetId;
}
