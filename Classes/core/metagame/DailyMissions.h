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
    const std::string ItemEarned = "ItemEarned";
    const std::string LevelFinished = "LevelFinished";
};

class DailyMissions
{    
public:
    static DailyMissions& getInstance();
    
    static const int MaxDayMissionsCount;
    
    bool loadMissions(const std::string &filename);
    
    void beforeRun();
    void afterRun(bool success);
    void event(const DailyTaskEvent &event);
    void rewardCompletedMission(DailyTaskBase::Ptr mission);
    void restoreTodayMissions(const std::string &id, VariablesSet progress, bool rewarded);
    void restoreRewardMissions(const std::string &id);
    
    void checkMissionsSwitchTime();
    void checkMissionsCompletness();
    void checkMastering();
    
    bool isTaskInRewardList(const std::string &id) const;
    
    const std::vector<DailyTaskBase::Ptr>& getTodayMissions() const;
    const std::vector<DailyTaskBase::Ptr>& getRewardMissions() const;
    const DailyTaskBase::Ptr findMissionInPool(const std::string &id) const;
    
private:
    DailyMissions();
    
    void refreshTodayMissions();
    
    int _mastering;
    
    std::vector<DailyTaskBase::Ptr> _pool;
    std::vector<DailyTaskBase::Ptr> _today;
    std::vector<DailyTaskBase::Ptr> _reward;
    std::vector<int> _masteringData;
};

#endif /* DailyMissions_hpp */
