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
    virtual void onRunFinished(bool success);
    
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
    virtual ~KillXEnemies();
    
    virtual void onEvent(const DailyTaskEvent &event) override;
    virtual bool checkCompletness() override;
    
private:
    const int _killsRequired;
};

class KillXEnemiesOnce : public KillXEnemies
{
public:
    static Ptr create(const BaseInfo &info, int killsRequired);
    
    KillXEnemiesOnce(const BaseInfo &info, int killsRequired);
    
    virtual void onRunBegan() override;
};

class CollectXCoins : public DailyTaskBase
{
public:
    static Ptr create(const BaseInfo &info, int coinsRequired);
    
    CollectXCoins(const BaseInfo &info, int killsRequired);
    virtual ~CollectXCoins();
    
    virtual void onEvent(const DailyTaskEvent &event) override;
    virtual bool checkCompletness() override;
    
private:
    const int _coinsRequired;
};

class CollectXCoinsOnce : public CollectXCoins
{
public:
    static Ptr create(const BaseInfo &info, int coinsRequired);
    
    CollectXCoinsOnce(const BaseInfo &info, int coinsRequired);
    
    virtual void onRunBegan() override;
};

class CollectXShards : public DailyTaskBase
{
public:
    static Ptr create(const BaseInfo &info, const std::string &shardId, int amount);
    
    CollectXShards(const BaseInfo &info, const std::string &shardId, int amount);
    
    virtual void onEvent(const DailyTaskEvent &event) override;
    virtual bool checkCompletness() override;
    
private:
    const int _amount;
    const std::string _shardId;
};

class CollectXItems : public DailyTaskBase
{
public:
    typedef std::pair<std::string, int> Data; // id - amount
    typedef std::vector<Data> IdList;
    
public:
    static Ptr create(const BaseInfo &info, IdList idList, const std::string &levelId);
    
    CollectXItems(const BaseInfo &info, IdList idList, const std::string &levelId);
    
    virtual void onEvent(const DailyTaskEvent &event) override;
    virtual bool checkCompletness() override;
    
private:
    const std::string _levelId;
    const IdList _itemsRequired;
};

class CompleteLevelWithoutHealthLooses : public DailyTaskBase
{
public:
    static Ptr create(const BaseInfo &info, const std::string &levelId);
    
    CompleteLevelWithoutHealthLooses(const BaseInfo &info, const std::string &levelId);
    
    virtual void onEvent(const DailyTaskEvent &event) override;
    virtual void onRunBegan() override;
    virtual void onRunFinished(bool success) override;
    virtual bool checkCompletness() override;
    virtual void flush() override;
    
private:
    const std::string _levelId;
    bool _successCheckFlag;
};

class PassXSquares : public DailyTaskBase
{
public:
    static Ptr create(const BaseInfo &info, int squares);
    
    PassXSquares(const BaseInfo &info, int squares);
    
    virtual void onEvent(const DailyTaskEvent &event) override;
    virtual bool checkCompletness() override;
    
private:
    const int _squaresRequired;
};

class FinishLevelDuringTheTime : public DailyTaskBase
{
public:
    static Ptr create(const BaseInfo &info, const std::string &levelId, float time);
    
    FinishLevelDuringTheTime(const BaseInfo &info, const std::string &levelId, float time);
    
    virtual void onEvent(const DailyTaskEvent &event) override;
    virtual void onRunBegan() override;
    virtual void onRunFinished(bool success) override;
    virtual bool checkCompletness() override;
    virtual void flush() override;
    
private:
    const std::string _levelId;
    const float _requiredTime;
    
    bool _status;
};

#endif /* DailyTask_hpp */
