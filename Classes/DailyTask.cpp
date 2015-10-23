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

// CompleteLevelWithoutHealthLooses

DailyTaskBase::Ptr CompleteLevelWithoutHealthLooses::create(const BaseInfo &info, const std::string &levelId)
{
    return std::make_shared<CompleteLevelWithoutHealthLooses>(info, levelId);
}

CompleteLevelWithoutHealthLooses::CompleteLevelWithoutHealthLooses(const BaseInfo &info, const std::string &levelId)
: DailyTaskBase(info, Tracking::DamageReceived)
, _levelId(levelId)
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
}

bool CompleteLevelWithoutHealthLooses::checkCompletness()
{
    LevelsCache &levels = LevelsCache::getInstance();
    FieldLevel::Ptr level = levels.getLevelById(_levelId).lock();
    
    if (!level) {
        WRITE_ERR("Invalid level with id: " + _levelId);
        return false;
    }
    
    return !_progress.getBool("damageReceived") && level->isStatus(FieldLevel::Status::COMPLETED);
}
