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
, _coinRewardForCompletition(0)
, _posOnMapX(0.0f)
, _posOnMapY(0.0f)
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
        WRITE_WARN("Unknown level status: " + str);
        return Status::UNLOCKED;
    }
}

std::string FieldLevel::statusToString(Status status)
{
    if (status == Status::LOCKED) {
        return "locked";
    } else if (status == Status::UNLOCKED) {
        return "unlocked";
    } else if (status == Status::COMPLETED) {
        return "completed";
    } else {
        WRITE_WARN("Unknown level status");
        return "unlocked";
    }
}

void FieldLevel::initFromXml(tinyxml2::XMLNode *node)
{
    release();
    
    float speed = 0.0f;
    
    auto head = node->ToElement();
    auto reward = head->FirstChildElement("Rewards");
    auto unlocks = head->FirstChildElement("Unlocks");
    auto description = head->FirstChildElement("Description");
    
    _id = head->Attribute("id");
    _status = stringToStatus(head->Attribute("status"));
    
    std::string condition = head->Attribute("condition");
    if (condition == "classic") {
        _victoryCondition = ClassicCondition::create(this);
    } else {
        WRITE_ERR("Unknown level victory condition: " + condition);
    }
    
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
                _construction.push_back(ConstructionInfo(type, id, 0, 0, speed));
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
                    _construction.push_back(ConstructionInfo(type, "", lower, upper, speed));
                } else {
                    int difficult = atoi(dif.c_str());
                    _construction.push_back(ConstructionInfo(type, "", difficult, difficult, speed));
                }
            }
        } else if (type == "Link") {
            std::string id = elem->Attribute("id");
            _construction.push_back(ConstructionInfo(type, id, 0, 0, speed));
        } else if (type == "LinkRand") {
            _construction.push_back(ConstructionInfo(type, "", 0, 0, speed));
        } else if (type == "Speed") {
            speed = elem->FloatAttribute("value");
        } else {
            WRITE_WARN("Unknown level entity: " + type);
        }
        elem = elem->NextSiblingElement();
    }
}

void FieldLevel::prepearForRun(Hero *hero)
{
    _lastSectorIndex = 0;
    _sectors.clear();
    _victoryCondition->init(hero);
    
    PresetsLoader &presets = PresetsLoader::getInstance();
    
    for (auto info : _construction) {
        if (info.type == "Preset") {
            if (info.id.empty()) {
                int difficult = misc::random(info.diffLower, info.diffUpper);
                addSector(presets.getRandomPresetWithDif(difficult), info.speed);
            } else {
                addSector(presets.getPresetById(info.id), info.speed);
            }
        } else if (info.type == "Link") {
            addSector(presets.getLinkById(info.id), info.speed);
        } else if (info.type == "LinkRand") {
            addSector(presets.getRandomLink(), info.speed);
        }
    }
}

void FieldLevel::restore(const SaveData &saveData)
{
    _status = saveData.status;
    for (Drop &drop : _drops) {
        for (const auto &fact : saveData.occurredDrop) {
            if (fact.first == drop.itemId) {
                drop.droped = fact.second;
            }
        }
    }
}

void FieldLevel::update(float dt)
{
    _victoryCondition->update(dt);
}

void FieldLevel::release()
{
    _sectors.clear();
    _construction.clear();
    _id.clear();
    _lastSectorIndex = 0;
}

void FieldLevel::setStatus(FieldLevel::Status status)
{
    _status = status;
}

void FieldLevel::setPosOnMap(float x, float y)
{
    _posOnMapX = x;
    _posOnMapY = y;
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

const std::string& FieldLevel::getId() const
{
    return _id;
}

const std::vector<std::string> FieldLevel::getUnlocks() const
{
    return _unlocks;
}

FieldLevel::Status FieldLevel::getStatus() const
{
    return _status;
}

int FieldLevel::getCoinsReward() const
{
    return _coinRewardForCompletition;
}

FieldLevel::SaveData FieldLevel::getSaveData() const
{
    SaveData data;
    data.status = _status;
    for (const Drop &drop : _drops) {
        auto fact = SaveData::DropFact(drop.itemId, drop.droped);
        data.occurredDrop.push_back(fact);
    }
    return data;
}

int FieldLevel::getSectorsAmount() const
{
    return (int)_sectors.size();
}

float FieldLevel::getMapX() const
{
    return _posOnMapX;
}

float FieldLevel::getMapY() const
{
    return _posOnMapY;
}

bool FieldLevel::isStatus(Status status)
{
    return _status == status;
}

bool FieldLevel::isFinished() const
{
    return !_victoryCondition->isResult(VictoryCondition::Result::NONE);
}

VictoryCondition::WeakPtr FieldLevel::getVictoryCondition() const
{
    return _victoryCondition;
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
