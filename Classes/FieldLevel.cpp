//
//  FieldLevel.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 08/08/15.
//
//

#include "FieldLevel.h"

#include "Utils.h"
#include "Log.h"

FieldLevel::FieldLevel()
: _lastSectorIndex(0)
, _status(Status::LOCKED)
, _posOnMapX(0.0f)
, _posOnMapY(0.0f)
, _coinRewardForCompletition(0)
{
}

FieldLevel::Ptr FieldLevel::create()
{
    return std::make_shared<FieldLevel>();
}

FieldLevel::Status FieldLevel::stringToStatus(const std::string &str)
{
    if (str == "locked") {
        return Status::LOCKED;
    } else if (str == "unlocked") {
        return Status::UNLOCKED;
    } else if (str == "completed") {
        return Status::COMPLETED;
    } else {
        WRITE_WARN("Unknown level status");
        return Status::UNLOCKED;
    }
}

void FieldLevel::initFromXml(tinyxml2::XMLNode *node)
{
    PresetsLoader &presets = PresetsLoader::getInstance();
    
    float speed = 0.0f;
    
    auto head = node->ToElement();
    auto reward = head->FirstChildElement("Rewards");
    auto unlocks = head->FirstChildElement("Unlocks");
    auto description = head->FirstChildElement("Description");
    
    _id = head->Attribute("id");
    _status = stringToStatus(head->Attribute("status"));
    _posOnMapX = head->FloatAttribute("mapX");
    _posOnMapY = head->FloatAttribute("mapY");
    
    if (reward) {
        _drops.clear();
        _coinRewardForCompletition = reward->IntAttribute("coins");
        auto elem = reward->FirstChildElement();
        while (elem) {
            Drop drop;
            drop.itemId = elem->Attribute("itemId");
            drop.chance = elem->IntAttribute("dropChance");
            drop.once = elem->BoolAttribute("dropOnce");
            _drops.push_back(drop);
            elem = elem->NextSiblingElement();
        }
    }
    
    if (unlocks) {
        _unlocks.clear();
        auto elem = unlocks->FirstChildElement();
        while (elem) {
            std::string levelId = elem->Attribute("id");
            _unlocks.push_back(levelId);
            elem = elem->NextSiblingElement();
        }
    }
    
    tinyxml2::XMLElement *elem = nullptr;
    if (description) {
        elem = description->FirstChildElement();
    }
    
    while (elem) {
        std::string type = elem->Name();
        if (type == "Preset") {
            if (elem->Attribute("id")) {
                std::string id = elem->Attribute("id");
                addSector(presets.getPresetById(id), speed);
            } else if (elem->Attribute("dif")) {
                std::string dif = elem->Attribute("dif");
                std::size_t pos = dif.find("-");
                if (pos != std::string::npos) {
                    std::size_t pos = dif.find("-");
                    std::string s0 = dif.substr(0, pos);
                    std::string s1 = dif.substr(pos+1, (dif.length()-1) - pos);
                    int lower = atoi(s0.c_str());
                    int upper = atoi(s1.c_str());
                    if (lower > upper) {std::swap(lower, upper);}
                    int difficult = misc::random(lower, upper);
                    addSector(presets.getRandomPresetWithDif(difficult), speed);
                } else {
                    int difficult = atoi(dif.c_str());
                    addSector(presets.getRandomPresetWithDif(difficult), speed);
                }
            }
        } else if (type == "Link") {
            std::string id = elem->Attribute("id");
            addSector(presets.getLinkById(id), speed);
        } else if (type == "LinkRand") {
            addSector(presets.getRandomLink(), speed);
        } else if (type == "Speed") {
            speed = elem->FloatAttribute("value");
        }
        elem = elem->NextSiblingElement();
    }
}

void FieldLevel::release()
{
    _sectors.clear();
    _id.clear();
    _lastSectorIndex = 0;
}

FieldSector::Ptr FieldLevel::getNextSector()
{
    if (_lastSectorIndex < _sectors.size()) {
        int index = _lastSectorIndex;
        _lastSectorIndex++;
        return _sectors[index];
    } else {
        return FieldSector::Ptr();
    }
}

FieldSector::Ptr FieldLevel::getSectorByIndex(int index)
{
    if (index >= 0 && index < _sectors.size()) {
        return _sectors[index];
    }
    return FieldSector::Ptr();
}

float FieldLevel::getPosOnMapX() const
{
    return _posOnMapX;
}

float FieldLevel::getPosOnMapY() const
{
    return _posOnMapY;
}

const std::string& FieldLevel::getId() const
{
    return _id;
}

bool FieldLevel::isStatus(Status status)
{
    return _status == status;
}

void FieldLevel::addSector(const Preset &preset, float speed)
{
    auto sector = FieldSector::create();
    sector->init(preset, speed);
    
    float ypos = 0.0f;
    if (!_sectors.empty()) {
        auto last = --_sectors.end();
        float lastYPos = (*last)->getY();
        ypos = lastYPos + (*last)->getHeight();
    }
    
    sector->setX(0.0f);
    sector->setY(ypos);
    
    _sectors.push_back(sector);
}
