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
, _runnigTimeSec(0.0f)
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
    
    if (description)
    {
        _info.enemiesAmount = description->IntAttribute("enemiesAmount");
        _info.lengthInSquares = description->IntAttribute("lengthInSquares");
        _info.runningSpeedBegin = description->FloatAttribute("runningSpeedBegin");
        _info.runningSpeedEnd = description->FloatAttribute("runningSpeedEnd");
        _info.enemiesDifficultCoeff = description->FloatAttribute("enemiesDifficultCoeff");
        
        std::string enemiesPool = description->Attribute("availableEnemiesPool");
        std::stringstream stream(enemiesPool);
        
        while (stream.good())
        {
            std::string substr;
            std::getline(stream, substr, ',');
            substr.erase(std::remove(substr.begin(), substr.end(), ' '), substr.end());
            _info.abaliableEnemies.push_back(substr);
        }
    }
}

void FieldLevel::prepearForRun(Hero *hero)
{
    _lastSectorIndex = 0;
    _runnigTimeSec = 0.0f;
    _sectors.clear();
    _victoryCondition->init(hero);
    
    PresetsLibrary &presetsLib = PresetsLibrary::getInstance();
    
    std::vector<Preset> route;
    
    int maxEnemiesPerSector = 5;
    int curEnemiesPerSector = maxEnemiesPerSector;
    
    const float minValuation = 0.5f;
    float targetValuation = 0.8f;
    float valuation = 0.0;
    
    // generate route with spawn points
    
    do
    {
        route.clear();
        int routeLen = 0;
        int spawnNum = 0;
        
        while (spawnNum <= _info.enemiesAmount)
        {
            // try to add some diversity by giving a chance to change required spawn amount for sector
            // chance to get smaller/bigger amount always multiplied by 0.4
            
            int amount = curEnemiesPerSector;
            int sign = misc::random(0, 100) < 50 ? 1 : -1;
            int chance = misc::random(0, 100);
            if (chance > 70)
            {
                int add = sign;
                float maxChance = 100;
                float probability = misc::random(0, maxChance);
                while (maxChance > 0)
                {
                    maxChance = maxChance * 0.4f;
                    
                    if (probability > maxChance) {
                        break;
                    } else {
                        add += sign;
                    }
                }
                
                amount = math::clamp(amount + add, 0, maxEnemiesPerSector);
            }
            
            Preset preset = presetsLib.getRandomWithEnemiesAmount(amount);
            
            if (isSuitablePreset(preset))
            {
                int mirrirChance = misc::random(0, 100);
                if (mirrirChance > 50) {
                    mirrorPreset(preset);
                }
                
                spawnNum += preset.enemiesAmount;
                routeLen += preset.squaresByHeight;
                route.push_back(preset);
            }
            
            if (routeLen > _info.lengthInSquares) {
                break;
            }
        }
        
        float routeLenValuation = 0.0f;
        float spawnNumValuation = 0.0f;
        
        routeLenValuation = (float)routeLen / (float)_info.lengthInSquares;
        spawnNumValuation = (float)spawnNum / (float)_info.enemiesAmount;
        
        routeLenValuation = math::clamp(routeLenValuation, 0.0f, 1.0f);
        spawnNumValuation = math::clamp(spawnNumValuation, 0.0f, 1.0f);
        
        valuation = spawnNumValuation * routeLenValuation;
        
        curEnemiesPerSector--;
        
        if (curEnemiesPerSector < 0) {
            targetValuation -= 0.5f;
            if (targetValuation < minValuation) {
                break;
            } else {
                curEnemiesPerSector = maxEnemiesPerSector;
            }
        }
        
    } while (valuation < targetValuation);
    
    Preset emptyPreset = presetsLib.getRandomWithEnemiesAmount(0);
    route.insert(route.begin(), emptyPreset);
    route.insert(route.end(), emptyPreset);
    
    // accomodate level with real enemies
    // overal enemies difficult mast be close as possible to overal level difficult
    
    std::multimap<std::string, std::string> enemies; // key - group, value - id
    std::vector<std::string> requiredEnemies;
    
    for (Preset &preset : route) {
        for (auto& spawn : preset.spawnMap) {
            requiredEnemies.push_back(spawn.group);
        }
    }
    
    GameInfo &gameinfo = GameInfo::getInstance();
    
    float totalDifficult = 0.0f;
    
    for (int k = 0; k < requiredEnemies.size(); k++)
    {
        std::string group = requiredEnemies[k];
        
        std::vector<std::string> enemiesForGroup;
        for (const std::string& id : _info.abaliableEnemies) {
            GameInfo::EnemyType type;
            type = gameinfo.getEnemyInfoByName(id);
            
            if (type.group == group) {
                enemiesForGroup.push_back(id);
            }
        }
        
        std::size_t lower = 0;
        std::size_t upper = enemiesForGroup.size() - 1;
        std::size_t index = misc::random(lower, upper);
        
        std::string id = enemiesForGroup.at(index);
        
        GameInfo::EnemyType type = gameinfo.getEnemyInfoByName(id);
        totalDifficult += (float)type.difficult;
        
        auto data = std::pair<std::string, std::string>(group, id);
        enemies.insert(data);
    }
    
    float difficultValuation = 0.0f;
    difficultValuation = totalDifficult / _info.enemiesDifficultCoeff;
    difficultValuation = math::clamp(difficultValuation, 0.0f, 1.0f);
    
    for (Preset &preset : route)
    {
        for (auto& spawn : preset.spawnMap)
        {
            auto bounds = enemies.equal_range(spawn.group);
            std::vector<std::pair<std::string, std::string>> sample;
            std::copy(bounds.first, bounds.second, std::back_inserter(sample));
            
            if (!sample.empty())
            {
                int min = 0;
                int max = sample.size() - 1;
                int index = misc::random(min, max);
                std::string id = sample[index].second;
                
                spawn.entityId = id;
                
                // remove enemy from list
                auto it = bounds.first;
                for (; it != bounds.second; ++it) {
                    if (it->second == id) {
                        enemies.erase(it);
                        break;
                    }
                }
            }
        }
        
        addSector(preset);
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
    
    _runnigTimeSec += dt;
}

void FieldLevel::release()
{
    _sectors.clear();
    _id.clear();
    _lastSectorIndex = 0;
    _runnigTimeSec = 0.0f;
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
    if ((std::size_t)_lastSectorIndex < _sectors.size()) {
        int index = _lastSectorIndex;
        _lastSectorIndex++;
        return _sectors[index];
    } else {
        return FieldSector::Ptr();
    }
}

FieldSector::Ptr FieldLevel::getSectorByIndex(int index)
{
	if (index >= 0 && (std::size_t)index < _sectors.size()) {
        return _sectors[index];
    }
    return FieldSector::Ptr();
}

const std::string& FieldLevel::getId() const
{
    return _id;
}

const std::vector<std::string>& FieldLevel::getUnlocks() const
{
    return _unlocks;
}

std::vector<std::string> FieldLevel::getPossibleLoot() const
{
    std::vector<std::string> loot;
    
    for (const Drop &drop : _drops) {
        if (drop.chance > 0) {
            if (drop.once && drop.droped) {
                continue;
            }
            loot.push_back(drop.itemId);
        }
    }
    
    return loot;
}

std::vector<std::string> FieldLevel::dropLoot()
{
    std::vector<std::string> loot;
    
    for (Drop &drop : _drops) {
        if (drop.once && drop.droped) {
            continue;
        }
        bool droped = misc::random(0, 100) <= drop.chance;
        if (droped) {
            loot.push_back(drop.itemId);
            drop.droped = true;
        }
    }
    
    return loot;
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

float FieldLevel::getRunningTime() const
{
    return _runnigTimeSec;
}

float FieldLevel::getRinningSpeedForPos(float ypos) const
{
    float s1 = _info.runningSpeedBegin;
    float s2 = _info.runningSpeedEnd;
    
    float squareSize = GameInfo::getInstance().getConstFloat("SQUARE_SIZE");
    float length = _info.lengthInSquares * squareSize;
    float t = ypos / length;
    t = math::clamp(t, 0.0f, 1.0f);
    
    float speed = math::lerp(s1, s2, t);
    
    return speed;
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

void FieldLevel::addSector(const Preset &preset)
{
    auto sector = FieldSector::create();
    sector->init(preset);
    
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

void FieldLevel::mirrorPreset(Preset &preset)
{
    for (auto &spawn : preset.spawnMap)
    {
        spawn.x = (preset.squaresByWidth - spawn.x) - 1;
    }
}

bool FieldLevel::isSuitablePreset(const Preset &preset) const
{
    GameInfo &gameinfo = GameInfo::getInstance();
    
    for (const auto& spawn : preset.spawnMap)
    {
        const std::string &group = spawn.group;
        
        bool found = false;
        
        for (const std::string &abaliable : _info.abaliableEnemies) {
            auto enemy = gameinfo.getEnemyInfoByName(abaliable);
            if (enemy.group == group) {
                found = true;
                break;
            }
        }
        
        if (!found) {
            return false;
        }
    }
    
    return true;
}
