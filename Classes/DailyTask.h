//
//  DailyTask.hpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 10/10/15.
//
//

#ifndef DailyTask_hpp
#define DailyTask_hpp

#include <memory>
#include <string>
#include <vector>

#include "VariablesSet.h"

class DailyTaskEvent
{
public:
    DailyTaskEvent(const std::string &id)
    : _id(id)
    {}
    
    inline bool is(const std::string &id) const {return id == _id;}
    VariablesSet data;
private:
    const std::string _id;
};

class DailyTaskBase
{
public:
    struct BaseInfo
    {
        int difficult;
        int required;
        int coinsReward;
        int lootRewardAmount;
        
        std::string id;
        std::string lootRewardId;
        std::string description;
        
        BaseInfo() = default;
    };
    
    typedef std::shared_ptr<DailyTaskBase> Ptr;
    typedef std::weak_ptr<DailyTaskBase> WeakPtr;
    
public:
    DailyTaskBase(const BaseInfo &info, const std::string &subscription);
    virtual ~DailyTaskBase();
    
    virtual void onEvent(const DailyTaskEvent &event) = 0;
    virtual bool checkCompletness() = 0;
    virtual void restore(VariablesSet progress, bool rewarded);
    virtual void flush();
    virtual void onRunBegan();
    
    void giveReward();
    
    bool isSubscribed(const DailyTaskEvent &event) const;
    bool isRewarded() const;
    
    const BaseInfo& getInfo() const;
    const VariablesSet& getProgress() const;
    
protected:
    BaseInfo _info;
    VariablesSet _progress;
    bool _rewarded;
    const std::string _subscription;
    
    friend class DailyMissions;
};

class KillXEnemies : public DailyTaskBase
{
public:
    static Ptr create(const BaseInfo &info, int killsRequired);
    
    KillXEnemies(const BaseInfo &info, int killsRequired);
    
    virtual void onEvent(const DailyTaskEvent &event) override;
    virtual bool checkCompletness() override;
    
private:
    const int _killsRequired;
};

class CollectXCoins : public DailyTaskBase
{
public:
    static Ptr create(const BaseInfo &info, int coinsRequired);
    
    CollectXCoins(const BaseInfo &info, int killsRequired);
    
    virtual void onEvent(const DailyTaskEvent &event) override;
    virtual bool checkCompletness() override;
    
private:
    const int _coinsRequired;
};

class CompleteLevelWithoutHealthLooses : public DailyTaskBase
{
public:
    static Ptr create(const BaseInfo &info, const std::string &levelId);
    
    CompleteLevelWithoutHealthLooses(const BaseInfo &info, const std::string &levelId);
    
    virtual void onEvent(const DailyTaskEvent &event) override;
    virtual void onRunBegan() override;
    virtual bool checkCompletness() override;
    
private:
    const std::string _levelId;
};

#endif /* DailyTask_hpp */
