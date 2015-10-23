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

namespace Tracking
{
    const std::string EnemyKilled = "EnemyKilled";
    const std::string SquarePassed = "SquarePassed";
    const std::string DamageReceived = "DamageReceived";
    const std::string CoinEarned = "CoinEarned";
    const std::string ItemEarned = "LootEarned";
};

class DailyMissions
{    
public:
    static DailyMissions& getInstance();
    
    static const int MaxDayMissionsCount;
    
    bool loadMissions(const std::string &filename);
    
    void beforeRun();
    void event(const DailyTaskEvent &event);
    void restoreTodayMissions(const std::string &id, VariablesSet progress, bool rewarded);
    
    void checkMissionsStatus();
    void checkMastering();
    
    const std::vector<DailyTaskBase::Ptr>& getTodayMissions() const;
    
private:
    DailyMissions();
    
    void refreshTodayMissions();
    
    int _mastering;
    
    std::vector<DailyTaskBase::Ptr> _pool;
    std::vector<DailyTaskBase::Ptr> _today;
    std::vector<int> _masteringData;
};

#endif /* DailyMissions_hpp */
