//
//  DailyTask.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 10/10/15.
//
//

#include "DailyTask.h"
#include "DailyMissions.h"
#include "PlayerInfo.h"
#include "LevelsCache.h"
#include "Store.h"
#include "Utils.h"
#include "Log.h"

DailyTaskBase::DailyTaskBase(const BaseInfo &info, const std::string &subscription)
: _info(info)
, _rewarded(false)
, _subscription(subscription)
{
}

DailyTaskBase::~DailyTaskBase()
{
}

void DailyTaskBase::restore(VariablesSet progress, bool rewarded)
{
    _progress = progress;
    _rewarded = rewarded;
}

void DailyTaskBase::flush()
{
    _progress.clear();
    _rewarded = false;
}

void DailyTaskBase::onRunBegan()
{
}

void DailyTaskBase::onRunFinished(bool success)
{
}

void DailyTaskBase::giveReward()
{
    PlayerInfo &player = PlayerInfo::getInstance();
    
    Item::Ptr item = Store::getInstance().getItemById(_info.lootRewardId);
    
    player.addCoins(_info.coinsReward);
    player.Inventory.add(item);
    
    _rewarded = true;
}

bool DailyTaskBase::isSubscribed(const DailyTaskEvent &event) const
{
    return event.is(_subscription);
}

bool DailyTaskBase::isRewarded() const
{
    return _rewarded;
}

const DailyTaskBase::BaseInfo& DailyTaskBase::getInfo() const
{
    return _info;
}

const VariablesSet& DailyTaskBase::getProgress() const
{
    return _progress;
}

// KillXEnemies

DailyTaskBase::Ptr KillXEnemies::create(const BaseInfo &info, int killsRequired)
{
    return std::make_shared<KillXEnemies>(info, killsRequired);
}

KillXEnemies::KillXEnemies(const BaseInfo &info, int killsRequired)
: DailyTaskBase(info, Tracking::EnemyKilled)
, _killsRequired(killsRequired)
{
}

KillXEnemies::~KillXEnemies()
{
}

void KillXEnemies::onEvent(const DailyTaskEvent &event)
{
    if (!checkCompletness())
    {
        int add = event.data.getInt("amount");
        _progress.incInt("kills", add);
    }
}

bool KillXEnemies::checkCompletness()
{
    return _progress.getInt("kills") >= _killsRequired;
}

// KillXEnemiesOnce

DailyTaskBase::Ptr KillXEnemiesOnce::create(const BaseInfo &info, int killsRequired)
{
    return std::make_shared<KillXEnemiesOnce>(info, killsRequired);
}

KillXEnemiesOnce::KillXEnemiesOnce(const BaseInfo &info, int killsRequired)
: KillXEnemies(info, killsRequired)
{
}

void KillXEnemiesOnce::onRunBegan()
{
    _progress.setInt("kills", 0);
}

// CollectXCoins

DailyTaskBase::Ptr CollectXCoins::create(const BaseInfo &info, int coinsRequired)
{
    return std::make_shared<CollectXCoins>(info, coinsRequired);
}

CollectXCoins::CollectXCoins(const BaseInfo &info, int coinsRequired)
: DailyTaskBase(info, Tracking::CoinEarned)
, _coinsRequired(coinsRequired)
{
}

CollectXCoins::~CollectXCoins()
{
}

void CollectXCoins::onEvent(const DailyTaskEvent &event)
{
    if (!checkCompletness())
    {
        int add = event.data.getInt("amount");
        _progress.incInt("coins", add);
    }
}

bool CollectXCoins::checkCompletness()
{
    return _progress.getInt("coins") >= _coinsRequired;
}

// CollectXCoinsOnce

DailyTaskBase::Ptr CollectXCoinsOnce::create(const BaseInfo &info, int coinsRequired)
{
    return std::make_shared<CollectXCoinsOnce>(info, coinsRequired);
}

CollectXCoinsOnce::CollectXCoinsOnce(const BaseInfo &info, int coinsRequired)
: CollectXCoins(info, coinsRequired)
{
}

void CollectXCoinsOnce::onRunBegan()
{
    _progress.setInt("coins", 0);
}

// CompleteLevelWithoutHealthLooses

DailyTaskBase::Ptr CompleteLevelWithoutHealthLooses::create(const BaseInfo &info, const std::string &levelId)
{
    return std::make_shared<CompleteLevelWithoutHealthLooses>(info, levelId);
}

CompleteLevelWithoutHealthLooses::CompleteLevelWithoutHealthLooses(const BaseInfo &info, const std::string &levelId)
: DailyTaskBase(info, Tracking::DamageReceived)
, _levelId(levelId)
, _successCheckFlag(false)
{
}

void CompleteLevelWithoutHealthLooses::onEvent(const DailyTaskEvent &event)
{
    if (!checkCompletness())
    {
        PlayerInfo &player = PlayerInfo::getInstance();
    
        std::string currentLevel = player.variables.getString(PlayerInfo::VarKeyLastPlayedLevel);
    
        if (currentLevel == _levelId)
        {
            bool damageReceived = event.data.getFloat("amount") < 0.0f;
            _progress.setBool("damageReceived", damageReceived);
        }
    }
}

void CompleteLevelWithoutHealthLooses::onRunBegan()
{
    _progress.setBool("damageReceived", false);
    _successCheckFlag = false;
}

void CompleteLevelWithoutHealthLooses::onRunFinished(bool success)
{
    PlayerInfo &player = PlayerInfo::getInstance();
    std::string currentLevel = player.variables.getString(PlayerInfo::VarKeyLastPlayedLevel);
    
    if (currentLevel == _levelId)
    {
        _successCheckFlag = success;
    }
}

bool CompleteLevelWithoutHealthLooses::checkCompletness()
{
    LevelsCache &levels = LevelsCache::getInstance();
    FieldLevel::Ptr level = levels.getLevelById(_levelId).lock();
    
    if (!level) {
        WRITE_ERR("Invalid level with id: " + _levelId);
        return false;
    }
    
    return !_progress.getBool("damageReceived") && _successCheckFlag;
}

void CompleteLevelWithoutHealthLooses::flush()
{
    DailyTaskBase::flush();
    _successCheckFlag = false;
}

// PassXSquares

DailyTaskBase::Ptr PassXSquares::create(const BaseInfo &info, int squares)
{
    return std::make_shared<PassXSquares>(info, squares);
}

PassXSquares::PassXSquares(const BaseInfo &info, int squares)
: DailyTaskBase(info, Tracking::SquarePassed)
, _squaresRequired(squares)
{
}

void PassXSquares::onEvent(const DailyTaskEvent &event)
{
    if (!checkCompletness())
    {
        _progress.incInt("squaresPassed");
    }
}

bool PassXSquares::checkCompletness()
{
    return _progress.getInt("squaresPassed") >= _squaresRequired;
}

// FinishLevelDuringTheTime

DailyTaskBase::Ptr FinishLevelDuringTheTime::create(const BaseInfo &info, const std::string &levelId, float time)
{
    return std::make_shared<FinishLevelDuringTheTime>(info, levelId, time);
}

FinishLevelDuringTheTime::FinishLevelDuringTheTime(const BaseInfo &info, const std::string &levelId, float time)
: DailyTaskBase(info, Tracking::LevelFinished)
, _requiredTime(time)
, _levelId(levelId)
, _status(false)
{
}

void FinishLevelDuringTheTime::onEvent(const DailyTaskEvent &event)
{
    float duration = event.data.getFloat("time");
    _progress.setFloat("time", duration);
}

void FinishLevelDuringTheTime::onRunBegan()
{
    flush();
}

void FinishLevelDuringTheTime::onRunFinished(bool success)
{
    PlayerInfo &player = PlayerInfo::getInstance();
    std::string currentLevel = player.variables.getString(PlayerInfo::VarKeyLastPlayedLevel);
    
    _status = success && _levelId == currentLevel && _progress.getFloat("time") <= _requiredTime;
}

bool FinishLevelDuringTheTime::checkCompletness()
{
    return _status;
}

void FinishLevelDuringTheTime::flush()
{
    DailyTaskBase::flush();
    
    _status = false;
    _progress.setFloat("time", 0.0f);
}

// CollectXShards

DailyTaskBase::Ptr CollectXShards::create(const BaseInfo &info, const std::string &shardId, int amount)
{
    return std::make_shared<CollectXShards>(info, shardId, amount);
}

CollectXShards::CollectXShards(const BaseInfo &info, const std::string &shardId, int amount)
: DailyTaskBase(info, Tracking::ItemEarned)
, _amount(amount)
, _shardId(shardId)
{
}

void CollectXShards::onEvent(const DailyTaskEvent &event)
{
    std::string earnedItemId = event.data.getString("id");
    
    if (earnedItemId == _shardId)
    {
        int earnedAmount = event.data.getInt("amount");
        if (earnedAmount <= 0) {
            WRITE_WARN("Earned negative amount of items.");
            return;
        }
        
        _progress.incInt("amount", earnedAmount);
    }
}

bool CollectXShards::checkCompletness()
{
    return _progress.getInt("amount") >= _amount;
}

// CollectXItems

DailyTaskBase::Ptr CollectXItems::create(const BaseInfo &info, IdList idList, const std::string &levelId)
{
    return std::make_shared<CollectXItems>(info, idList, levelId);
}

CollectXItems::CollectXItems(const BaseInfo &info, IdList idList, const std::string &levelId)
: DailyTaskBase(info, Tracking::ItemEarned)
, _levelId(levelId)
, _itemsRequired(idList)
{
}

void CollectXItems::onEvent(const DailyTaskEvent &event)
{
    if (!checkCompletness())
    {
        PlayerInfo &player = PlayerInfo::getInstance();
        std::string currentLevel = player.variables.getString(PlayerInfo::VarKeyLastPlayedLevel);
    
        if (currentLevel != _levelId) {
            return;
        }
    
        std::string earnedItemId = event.data.getString("id");
    
        for (Data data : _itemsRequired)
        {
            if (data.first == earnedItemId)
            {
                int amount = event.data.getInt("amount");
                _progress.incInt(data.first, amount);
            
                break;
            }
        }
    }
}

bool CollectXItems::checkCompletness()
{
    bool result = true;
    
    for (Data data : _itemsRequired)
    {
        std::string id = data.first;
        int required = data.second;
        
        if (_progress.getInt(id, 0) < required) {
            result = false;
            break;
        }
    }
    
    return result;
}
