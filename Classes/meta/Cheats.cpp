//
//  Cheats.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 20/12/15.
//
//

#include "Cheats.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "PluginChartboost/PluginChartboost.h"
#endif

#include "GameInfo.h"
#include "PlayerInfo.h"
#include "Store.h"
#include "LevelsCache.h"

Cheats& Cheats::getInstance()
{
    static Cheats cheats;
    return cheats;
}

void Cheats::addCheat(const std::string &id, const std::string &title, std::function<void()> func)
{
    if (!isExist(id))
    {
        Cheats::Info desc;
        desc.id = id;
        desc.title = title;
        desc.func = func;
        
        _cheats[id] = desc;
    }
}

void Cheats::runCheat(const std::string &id)
{
    auto it = _cheats.find(id);
    if (it != _cheats.end()) {
        it->second.func();
    }
}

bool Cheats::isExist(const std::string &id) const
{
    auto it = _cheats.find(id);
    return it != _cheats.end();
}

void Cheats::clearAll()
{
    _cheats.clear();
}

const std::map<std::string, Cheats::Info>& Cheats::getAll() const
{
    return _cheats;
}

Cheats::Cheats()
{
}

void Cheats::initBasicCheats()
{
    Cheats &cheats = Cheats::getInstance();
    
    // add 10K coins
    std::function<void()> coins = []()
    {
        const int coins = 10000;
        PlayerInfo::getInstance().addCoins(coins);
    };
    
    // add 3 shards of each crystalls
    std::function<void()> shards = []()
    {
        PlayerInfo &player = PlayerInfo::getInstance();
            
        Store &store = Store::getInstance();
        Store::Items items = store.getItemsWithType(Item::Type::CRYSTALL);
            
        for (Item::Ptr item : items) {
            Crystall* crystall = Crystall::cast(item);
                
            Item::WeakPtr shard = store.getItemById(crystall->getShardId());
                
            player.Inventory.add(shard, 3);
        }
    };
    
    // unlock all levels
    std::function<void()> unlock = []()
    {
        LevelsCache &levels = LevelsCache::getInstance();
            
        for (int k = 0; k < levels.getLevelsAmount(); k++) {
            FieldLevel::Ptr level = levels.getLevelByIndex(k).lock();
            level->setStatus(FieldLevel::Status::UNLOCKED);
        }
    };

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    std::function<void()> ad_chartboost_interstitial = []()
    {
        sdkbox::PluginChartboost::show("Interstitial");
    };
    
    std::function<void()> ad_chartboost_rewarded_video = []()
    {
        sdkbox::PluginChartboost::show("RewardedVideo");
    };
    
    std::function<void()> ad_chartboost_video = []()
    {
        sdkbox::PluginChartboost::show("Video");
    };

    std::function<void()> ad_chartboost_moreapp = []()
    {
        sdkbox::PluginChartboost::show("MoreApp");
    };
    
    cheats.addCheat("ad_chartboost_interstitial", "Ad Chartboost Interstitial", ad_chartboost_interstitial);
    cheats.addCheat("ad_chartboost_rewarded_video", "Ad Chartboost Rewarded Video", ad_chartboost_rewarded_video);
    cheats.addCheat("ad_chartboost_video", "Ad Chartboost Video", ad_chartboost_video);
    cheats.addCheat("ad_chartboost_moreapp", "Ad Chartboost MoreApp", ad_chartboost_moreapp);
#endif
    
    cheats.addCheat("add_coins", "Add 10'000 coins", coins);
    cheats.addCheat("add_shard", "Add 3 shards of each crystalls", shards);
    cheats.addCheat("unlock_levels", "Unlock all levels", unlock);
}
