//
//  MapPlayerMark.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 30/08/15.
//
//

#include "MapPlayerMark.h"

#include "GameInfo.h"
#include "PlayerInfo.h"
#include "LevelsCache.h"

MapPlayerMark* MapPlayerMark::create()
{
    MapPlayerMark *mark = new MapPlayerMark();
    if (mark && mark->init()) {
        mark->autorelease();
    } else {
        delete mark;
        mark = nullptr;
    }
    return mark;
}

MapPlayerMark::MapPlayerMark()
{
}

MapPlayerMark::~MapPlayerMark()
{
}

bool MapPlayerMark::init()
{
    if (!cocos2d::ui::Widget::init()) {
        return false;
    }
    
    _mark = cocos2d::Sprite::create("map/map_player.png");
    _mark->setScale(1.5f);
    _mark->setAnchorPoint(cocos2d::Vec2(0.5f, -0.15f));
    
    addChild(_mark);
    
    refreshPosition();
    
    return true;
}

void MapPlayerMark::refreshPosition()
{
    LevelsCache &levels = LevelsCache::getInstance();
    PlayerInfo &player = PlayerInfo::getInstance();

    std::string lastAttachedLevelId = player.variables.getString("LevelMarkAttachId");
    std::string lastCompletedId = player.variables.getString("LastCompletedLevel");

    if (lastCompletedId.empty() && lastAttachedLevelId.empty()) {
        // first game launch
        FieldLevel::Ptr levelPtr = levels.getLevelByIndex(0).lock();
        setPosition(cocos2d::Vec2(levelPtr->getMapX(), levelPtr->getMapY()));
    }
    else {
        FieldLevel::Ptr prevLevel = levels.getLevelById(lastCompletedId).lock();
        FieldLevel::Ptr nextLevel = nullptr;

        // determin valid next level
        auto unlocks = prevLevel->getUnlocks();

        for (auto itr = unlocks.begin(); itr != unlocks.end(); itr++)
        {
            const std::string id = (*itr);
            FieldLevel::Ptr temp = levels.getLevelById(id).lock();

            // avoid spinoff levels
            if (!temp->getUnlocks().empty()) {
                nextLevel = temp;
                break;
            }
        }

        if (nextLevel) {
            cocos2d::Vec2 p0(prevLevel->getMapX(), prevLevel->getMapY());
            cocos2d::Vec2 p1(nextLevel->getMapX(), nextLevel->getMapY());
            
            if (nextLevel->getId() != lastAttachedLevelId) {
                setPosition(p0);
                auto move = cocos2d::MoveTo::create(2.0f, p1);
                runAction(move);
            }
            else {
                setPosition(p1);
            }

            player.variables.setString("LevelMarkAttachId", nextLevel->getId());
            player.save();
        }
    }
}
