//
//  DailyTask.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 10/10/15.
//
//

#include "DailyTask.h"
#include "DailyMissions.h"
#include "Utils.h"
#include "Log.h"

// DailyTask

DailyTask::DailyTask(const Info &info)
: _info(info)
{
}

DailyTask::~DailyTask()
{
}

DailyTask::Ptr DailyTask::create(const Info &info)
{
    return std::make_shared<DailyTask>(info);
}

bool DailyTask::checkCompletness()
{
    DailyMissions &daily = DailyMissions::getInstance();
    
    int current = daily.statistics.getInt(_info.tracking, -1);
    
    return current == _info.required;
}

int DailyTask::getDifficult() const
{
    return _info.difficult;
}

int DailyTask::getCoinsReward() const
{
    return _info.coinsReward;
}

std::string DailyTask::getDescription() const
{
    return _info.description;
}
