//
//  DailyMissions.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 07/10/15.
//
//

#include "DailyMissions.h"
#include "PlayerInfo.h"
#include "GameInfo.h"
#include "Utils.h"
#include "Log.h"

#include "cocos2d.h"
#include "tinyxml2/tinyxml2.h"
#include <ctime>
#include <sstream>

const int DailyMissions::MaxDayMissionsCount = 3;

DailyMissions& DailyMissions::getInstance()
{
    static DailyMissions dailyMissions;
    return dailyMissions;
}

DailyMissions::DailyMissions()
: _mastering(1)
{
}

static DailyTaskBase::BaseInfo taskInfoFromXml(tinyxml2::XMLElement *elem)
{
    DailyTaskBase::BaseInfo info;
    
    info.id = elem->Attribute("id");
    info.difficult = elem->IntAttribute("difficult");
    info.required = elem->IntAttribute("required");
    info.coinsReward = elem->IntAttribute("coinsReward");
    info.lootRewardId = elem->Attribute("lootRewardId");
    info.lootRewardAmount = elem->IntAttribute("lootRewardAmount");
    info.description = elem->Attribute("description");
    
    return info;
}

bool DailyMissions::loadMissions(const std::string &filename)
{
    cocos2d::FileUtils *fileUtils = cocos2d::FileUtils::getInstance();
    std::string path = fileUtils->fullPathForFilename(filename);
    std::string buff = fileUtils->getStringFromFile(path);
    
    tinyxml2::XMLDocument document;
    tinyxml2::XMLError result = document.Parse(buff.c_str());
    
    if (result == tinyxml2::XMLError::XML_SUCCESS || result == tinyxml2::XMLError::XML_NO_ERROR)
    {
        _pool.clear();
        
        auto root = document.RootElement();
        auto masteringNode = root->FirstChildElement("MasteringSettings");
        auto missionsNode = root->FirstChildElement("MissionsPool");
        
        auto masteringElem = masteringNode->FirstChildElement();
        while (masteringElem)
        {
            int missions = masteringElem->IntAttribute("missions");
            _masteringData.push_back(missions);
            masteringElem = masteringElem->NextSiblingElement();
        }
        
        auto taskNode = missionsNode->FirstChildElement();
        while (taskNode)
        {
            std::string name = taskNode->Name();
            
            DailyTaskBase::BaseInfo info = taskInfoFromXml(taskNode);
            DailyTaskBase::Ptr task = nullptr;
            
            if (name == "KillXEnemies") {
                int amount = taskNode->IntAttribute("killsRequired");
                task = KillXEnemies::create(info, amount);
            } else if (name == "KillXEnemiesOnce") {
                int amount = taskNode->IntAttribute("killsRequired");
                task = KillXEnemiesOnce::create(info, amount);
            } else if (name == "CollectXCoins") {
                int amount = taskNode->IntAttribute("coinsRequired");
                task = CollectXCoins::create(info, amount);
            } else if (name == "CollectXCoinsOnce") {
                int amount = taskNode->IntAttribute("coinsRequired");
                task = CollectXCoinsOnce::create(info, amount);
            } else if (name == "CollectXShards") {
                std::string shardId = taskNode->Attribute("shardId");
                int amount = taskNode->IntAttribute("shardsAmount");
                task = CollectXShards::create(info, shardId, amount);
            } else if (name == "CollectXItems") {
                CollectXItems::IdList list;
                std::string levelId = taskNode->Attribute("levelId");
                auto listNode = taskNode->FirstChildElement("Items");
                if (listNode) {
                    auto node = listNode->FirstChildElement();
                    while (node) {
                        CollectXItems::Data data;
                        data.first = node->Name();
                        data.second = 1;
                        node->QueryIntAttribute("amount", &data.second);
                        list.push_back(data);
                        node = node->NextSiblingElement();
                    }
                }
                task = CollectXItems::create(info, list, levelId);
            } else if (name == "CompleteLevelWithoutHealthLooses") {
                std::string levelId = taskNode->Attribute("levelId");
                task = CompleteLevelWithoutHealthLooses::create(info, levelId);
            } else if (name == "PassXSquares") {
                int amount = taskNode->IntAttribute("squaresRequired");
                task = PassXSquares::create(info, amount);
            } else if (name == "FinishLevelDuringTheTime") {
                float time = taskNode->FloatAttribute("time");
                std::string levelId = taskNode->Attribute("levelId");
                task = FinishLevelDuringTheTime::create(info, levelId, time);
            } else {
                WRITE_WARN("Unknown task type: " + name);
            }
            
            if (task)
            {
                _pool.push_back(task);
            }
            
            taskNode = taskNode->NextSiblingElement();
        }
        
        WRITE_INIT("Daily missions loaded successfully.");
    }
    else
    {
        WRITE_ERR("Failed to load daily missions.");
    }
    
    return false;
}

void DailyMissions::beforeRun()
{
    for (DailyTaskBase::Ptr task : _today)
    {
        if (!task->isRewarded()) {
            task->onRunBegan();
        }
    }
}

void DailyMissions::afterRun(bool success)
{
    for (DailyTaskBase::Ptr task : _today)
    {
        if (!task->isRewarded()) {
            task->onRunFinished(success);
        }
    }
    
    checkMissionsCompletness();
}

void DailyMissions::event(const DailyTaskEvent &event)
{
    for (DailyTaskBase::Ptr task : _today)
    {
        if (task->isSubscribed(event)) {
            task->onEvent(event);
        }
    }
}

void DailyMissions::rewardCompletedMission(DailyTaskBase::Ptr mission)
{
    for (auto iter = _reward.begin(); iter != _reward.end();)
    {
        DailyTaskBase::Ptr task = (*iter);
        if (task == mission)
        {
            task->giveReward();
            
            iter = _reward.erase(iter);
            break;
        }
        else
        {
            iter++;
        }
    }
}

void DailyMissions::restoreTodayMissions(const std::string &id, VariablesSet progress, bool rewarded)
{
    DailyTaskBase::Ptr task = nullptr;
    
    for (DailyTaskBase::Ptr ref : _pool) {
        if (ref->getInfo().id == id) {
            task = ref;
            break;
        }
    }
    
    if (task) {
        task->restore(progress, rewarded);
        _today.push_back(task);
    } else {
        WRITE_WARN("Trying to restore unknown task with id:" + id);
    }
}

void DailyMissions::restoreRewardMissions(const std::string &id)
{
    bool ok = true;
    
    for (DailyTaskBase::Ptr task : _reward)
    {
        if (task->getInfo().id == id) {
            ok = false;
            WRITE_WARN("Missions already restored.");
            break;
        }
    }
    
    if (ok)
    {
        auto task = findMissionInPool(id);
        if (task)
        {
            _reward.push_back(task);
        }
    }
}

void DailyMissions::checkMissionsSwitchTime()
{
    GameInfo &gameinfo = GameInfo::getInstance();
    PlayerInfo &player = PlayerInfo::getInstance();
    
    std::string duration_string = gameinfo.getConstString("DAILY_MISSIONS_UPDATE_DURATION", "0");
    std::string timestamp_string = player.variables.getString(PlayerInfo::VarKeyDailyTimestamp, "0");
    
    long duration = std::stol(duration_string);
    long timestamp = std::stol(timestamp_string);
    
    long time = (long)std::time(0);
    
    if (time > timestamp + duration)
    {
        // remove completed missions
        for (auto it = _today.begin(); it != _today.end();)
        {
            auto task = (*it);
            if (task->isRewarded()) {
                it = _today.erase(it);
            } else {
                it++;
            }
        }
        
        if (_today.size() < MaxDayMissionsCount)
        {
            refreshTodayMissions();
        }
        
        std::ostringstream stream;
        stream << time;
        std::string time_string = stream.str();
        
        player.variables.setString(PlayerInfo::VarKeyDailyTimestamp, time_string);
        player.save();
    }
}

void DailyMissions::checkMissionsCompletness()
{
    PlayerInfo &player = PlayerInfo::getInstance();

    bool hasCompleted = false;
    
    for (DailyTaskBase::Ptr task : _today)
    {
        if (task->checkCompletness() && !task->isRewarded() && !isTaskInRewardList(task->getInfo().id))
        {
            player.variables.incInt(PlayerInfo::VarKeyDailyCompleted);
            
            _reward.push_back(task);
            hasCompleted = true;
        }
    }
    
    if (hasCompleted)
    {
        checkMastering();
        player.save();
    }
}

void DailyMissions::checkMastering()
{
    PlayerInfo &player = PlayerInfo::getInstance();
    int missions = player.variables.getInt(PlayerInfo::VarKeyDailyCompleted, 0);
    int mastering = 1;
    
    for (int count : _masteringData)
    {
        missions -= count;
        if (missions >= 0) {
            mastering += 1;
        } else {
            break;
        }
    }
    
    _mastering = mastering;
}

void DailyMissions::refreshTodayMissions()
{
    std::vector<DailyTaskBase::Ptr> cach;
    for (DailyTaskBase::Ptr task : _pool) {
        if (task->getInfo().difficult == _mastering) {
            cach.push_back(task);
        }
    }
    
    if (cach.empty())
    {
        WRITE_WARN("Can't find suitable missions for mastering");
        return;
    }
    
    while (_today.size() < MaxDayMissionsCount)
    {
        int index = misc::random(0, cach.size() - 1);
        bool shouldAdd = true;
        
        DailyTaskBase::Ptr expectant = cach[index];
        
        for (DailyTaskBase::Ptr task : _today) {
            if (expectant == task) {
                shouldAdd = false;
                break;
            }
        }
        
        if (shouldAdd)
        {
            expectant->flush();
            _today.push_back(expectant);
        }
    }
}

const DailyTaskBase::Ptr DailyMissions::findMissionInPool(const std::string &id) const
{
    DailyTaskBase::Ptr result = nullptr;
    
    for (DailyTaskBase::Ptr task : _pool)
    {
        if (task->getInfo().id == id)
        {
            result = task;
            break;
        }
    }
    
    if (!result)
    {
        WRITE_WARN("Failt to find dailt task with id: " + id);
    }
    
    return result;
}

bool DailyMissions::isTaskInRewardList(const std::string &id) const
{
    for (DailyTaskBase::Ptr task : _reward) {
        if (task->getInfo().id == id) {
            return true;
        }
    }
    return false;
}

const std::vector<DailyTaskBase::Ptr>& DailyMissions::getTodayMissions() const
{
    return _today;
}

const std::vector<DailyTaskBase::Ptr>& DailyMissions::getRewardMissions() const
{
    return _reward;
}
