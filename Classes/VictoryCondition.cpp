//
//  VictoryCondition.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 27/08/15.
//
//

#include "VictoryCondition.h"

#include "Hero.h"
#include "FieldLevel.h"

VictoryCondition::VictoryCondition(FieldLevel *level)
: _hero(nullptr)
, _level(level)
, _result(Result::NONE)
{
}

VictoryCondition::~VictoryCondition()
{
}

void VictoryCondition::init(Hero *hero)
{
    if (hero) {
        _hero = hero;
    }
}

void VictoryCondition::update(float dt)
{
}

bool VictoryCondition::isResult(VictoryCondition::Result result) const
{
    return _result == result;
}

// ClassicVictory

ClassicCondition::Ptr ClassicCondition::create(FieldLevel *level)
{
    return std::make_shared<ClassicCondition>(level);
}

ClassicCondition::ClassicCondition(FieldLevel *level)
: VictoryCondition(level)
{
}

void ClassicCondition::update(float dt)
{
    if (isResult(Result::NONE))
    {
        int lastIndex = _level->getSectorsAmount() - 1;
        auto sector = _level->getSectorByIndex(lastIndex);
    
        // check for player reach the area if the last sector.
        // last sector is the finalizing sector and must be always empty
        if (_hero->getPositionY() >= sector->getY()) {
            _result = Result::VIRTORY;
        }
    }
}
