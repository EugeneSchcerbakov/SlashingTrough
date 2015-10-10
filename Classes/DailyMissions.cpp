//
//  DailyMissions.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 07/10/15.
//
//

#include "DailyMissions.h"

DailyMissions& DailyMissions::getInstance()
{
    static DailyMissions dailyMissions;
    return dailyMissions;
}

DailyMissions::DailyMissions()
{
}

void DailyMissions::prepareForLevel(const std::string id)
{
    _lastLevelId = id;
    
    statistics.setInt(Tracking::KillsForSingleRun, 0);
    statistics.setInt(Tracking::CoinsForSingleRun, 0);
    statistics.setFloat(Tracking::DamageReceivedForSingleRun, 0.0f);
}

void DailyMissions::clearStatistics()
{
    statistics.clear();
}