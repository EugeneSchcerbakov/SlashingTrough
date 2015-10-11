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

class DailyTask
{
public:
    struct Info
    {
        int difficult;
        int required;
        int coinsReward;
        int lootRewardAmount;
        
        std::string lootRewardId;
        std::string description;
        std::string tracking;
        
        Info() = default;
    };
    
    typedef std::shared_ptr<DailyTask> Ptr;
    typedef std::weak_ptr<DailyTask> WeakPtr;
    
    static Ptr create(const Info &info);
    
public:
    DailyTask(const Info &info);
    virtual ~DailyTask();
    
    bool checkCompletness();
    
    int getDifficult() const;
    int getCoinsReward() const;
    std::string getDescription() const;
    
private:
    Info _info;
};

#endif /* DailyTask_hpp */
