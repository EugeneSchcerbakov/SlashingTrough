//
//  DailyMissions.hpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 07/10/15.
//
//

#ifndef DailyMissions_hpp
#define DailyMissions_hpp

#include "DailyTask.h"
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
    
    bool loadMissions(const std::string &filename);
    
    void prepareForRun();
    void clearStatistics();
    
    void checkAfterRun();
    void checkSwitchDay();
    
    VariablesSet statistics;
    
private:
    DailyMissions();
    
    std::vector<DailyTask::Ptr> _pool;
    std::vector<DailyTask::Ptr> _today;
};

#endif /* DailyMissions_hpp */
