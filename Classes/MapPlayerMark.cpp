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
    GameInfo &gameinfo = GameInfo::getInstance();
    LevelsCache &levels = LevelsCache::getInstance();
    PlayerInfo &player = PlayerInfo::getInstance();
    
    std::string lastCompleted = player.getLastCompletedLevelId();
    
    if (lastCompleted.empty()) {
        // probably player doesn't complete any level by now
        auto level = levels.getLevelByIndex(0).lock();
        setPosition(cocos2d::Vec2(level->getMapX(), level->getMapY()));
    } else {
        auto level = levels.getLevelById(lastCompleted).lock();
        std::string prev = gameinfo.getGlobalString("LAST_COMPLETED_LEVEL");
        if (!prev.empty() && lastCompleted != prev) {
            auto prevLevel = levels.getLevelById(prev).lock();
            cocos2d::Vec2 p0(prevLevel->getMapX(), prevLevel->getMapY());
            cocos2d::Vec2 p1(level->getMapX(), level->getMapY());
            setPosition(p0);
            auto move = cocos2d::MoveTo::create(2.0f, p1);
            runAction(move);
            gameinfo.setGlobalString("LAST_COMPLETED_LEVEL", lastCompleted);
        } else {
            setPosition(cocos2d::Vec2(level->getMapX(), level->getMapY()));
            gameinfo.setGlobalString("LAST_COMPLETED_LEVEL", lastCompleted);
        }
    }
}
