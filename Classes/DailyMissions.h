//
//  DailyMissions.hpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 07/10/15.
//
//

#ifndef DailyMissions_hpp
#define DailyMissions_hpp

#include "VariablesSet.h"

namespace Tracking
{
    const std::string TotalEnemiesKilled = "TotalEnemiesKilled";
    const std::string TotalCoinsRewarded = "TotalCoinsRewarded";
    const std::string TotalSquarePassed = "TotalSquarePassed";
    const std::string LevelCompleteTime = "LevelCompleteTime";
    const std::string DamageReceivedForSingleRun = "DamageReceivedForSingleRun";
    const std::string KillsForSingleRun = "KillsForSingleRun";
    const std::string CoinsForSingleRun = "CoinsForSingleRun";
};

class DailyMissions
{
public:
    static DailyMissions& getInstance();
    
    DailyMissions();
    
    void prepareForLevel(const std::string id);
    void clearStatistics();

    VariablesSet statistics;
    
private:
    std::string _lastLevelId;
};

#endif /* DailyMissions_hpp */
