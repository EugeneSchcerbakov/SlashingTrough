//
//  DailyMissions.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 07/10/15.
//
//

#include "DailyMissions.h"
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

static std::string getTrackingForTask(const std::string &type)
{
    if (type == "KillXEnemies") {
        return Tracking::TotalEnemiesKilled;
    } else if (type == "CollectXCoins") {
        return Tracking::TotalCoinsRewarded;
    } else {
        WRITE_WARN("Unknown task type:" + type);
        return "";
    }
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
        
        auto missionNode = missionsNode->FirstChildElement();
        while (missionNode)
        {
            DailyTask::Info info;
            info.tracking = getTrackingForTask(missionNode->Name());
            info.difficult = missionNode->IntAttribute("difficult");
            info.required = missionNode->IntAttribute("required");
            info.coinsReward = missionNode->IntAttribute("coinsReward");
            info.lootRewardId = missionNode->Attribute("lootRewardId");
            info.lootRewardAmount = missionNode->IntAttribute("lootRewardAmount");
            info.description = missionNode->Attribute("description");
            
            DailyTask::Ptr task = DailyTask::create(info);
            _pool.push_back(task);
            
            missionNode = missionNode->NextSiblingElement();
        }
        
        WRITE_INIT("Daily missions loaded successfully.");
    }
    else
    {
        WRITE_ERR("Failed to load daily missions.");
    }
    
    return false;
}

void DailyMissions::prepareForRun()
{
    statistics.setInt(Tracking::KillsForSingleRun, 0);
    statistics.setInt(Tracking::CoinsForSingleRun, 0);
    statistics.setInt(Tracking::DamageReceivedForSingleRun, 0);
}

void DailyMissions::clearStatistics()
{
    statistics.clear();
}

void DailyMissions::checkAfterRun()
{
    
}

void DailyMissions::checkSwitchDay()
{
    _today = _pool;
}

const std::vector<DailyTask::Ptr>& DailyMissions::getTodayMissions() const
{
    return _today;
}
