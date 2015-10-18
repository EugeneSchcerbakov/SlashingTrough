//
//  DailyMissions.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 07/10/15.
//
//

#include "DailyMissions.h"
#include "PlayerInfo.h"
#include "Utils.h"
#include "Log.h"

#include "cocos2d.h"
#include "tinyxml2/tinyxml2.h"

DailyMissions& DailyMissions::getInstance()
{
    static DailyMissions dailyMissions;
    return dailyMissions;
}

DailyMissions::DailyMissions()
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
        
        auto taskNode = missionsNode->FirstChildElement();
        while (taskNode)
        {
            std::string name = taskNode->Name();
            
            DailyTaskBase::BaseInfo info = taskInfoFromXml(taskNode);
            DailyTaskBase::Ptr task = nullptr;
            
            if (name == "KillXEnemies") {
                int amount = taskNode->IntAttribute("killsRequired");
                task = KillXEnemies::create(info, amount);
            } else if (name == "CollectXCoins") {
                int amount = taskNode->IntAttribute("coinsRequired");
                task = CollectXCoins::create(info, amount);
            } else if (name == "CompleteLevelWithoutHealthLooses") {
                std::string levelId = taskNode->Attribute("levelId");
                task = CompleteLevelWithoutHealthLooses::create(info, levelId);
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
        task->onRunBegan();
    }
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

void DailyMissions::checkMissionsStatus()
{
    if (_today.empty())
    {
        generateTodaysMissions();
    }
}

void DailyMissions::generateTodaysMissions()
{
    _today.clear();
    
    PlayerInfo &player = PlayerInfo::getInstance();
    
    int mastering = player.variables.getInt(PlayerInfo::VarDailyMastering, -1);
    if (mastering <= 0) {
        mastering = 1;
        player.variables.setInt(PlayerInfo::VarDailyMastering, mastering);
        player.save();
    }
    
    std::vector<DailyTaskBase::Ptr> cach;
    for (DailyTaskBase::Ptr task : _pool) {
        if (task->getInfo().difficult == mastering) {
            cach.push_back(task);
        }
    }
    
    const std::size_t maxGoals = 3;
    
    while (_today.size() < maxGoals)
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
            _today.push_back(expectant);
        }
    }
}

const std::vector<DailyTaskBase::Ptr>& DailyMissions::getTodayMissions() const
{
    return _today;
}
