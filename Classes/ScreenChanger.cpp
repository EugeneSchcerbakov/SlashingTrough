//
//  ScreenСhanger.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 26/08/15.
//
//

#include "ScreenChanger.h"

#include "GameScene.h"
#include "StartInterface.h"
#include "StoreInterface.h"
#include "MapInterface.h"
#include "StatisticsInterface.h"
#include "Log.h"

ScreenChanger::Screen ScreenChanger::START = "StartScreen";
ScreenChanger::Screen ScreenChanger::STORE = "StoreScreen";
ScreenChanger::Screen ScreenChanger::MAP = "MapScreen";
ScreenChanger::Screen ScreenChanger::STATISTICS = "StatisticsScreen";
ScreenChanger::Screen ScreenChanger::GAME = "GameScreen";

static void changeScene(cocos2d::Scene *scene, ScreenChanger::Screen screen)
{
    if (!scene) {
        return;
    }
    
    auto director = cocos2d::Director::getInstance();
    
    std::string log = "";
    scene->setName(screen);
    
    auto trans = cocos2d::TransitionFade::create(0.5f, scene, cocos2d::Color3B::BLACK);
    
    if (director->getRunningScene()) {
        director->replaceScene(trans);
        
        std::string prev = director->getRunningScene()->getName();
        log = cocos2d::StringUtils::format("%s changed to %s.", prev.c_str(), screen.c_str());
    } else {
        director->runWithScene(scene);
        
        log = cocos2d::StringUtils::format("%s opened.", screen.c_str());
    }
    
    if (!log.empty()) {
        WRITE_LOG(log);
    }
}

void ScreenChanger::changeScreen(ScreenChanger::Screen screen)
{
    cocos2d::Scene *scene = nullptr;
    std::string log = "";
    
    if (screen == ScreenChanger::START) {
        scene = StartInterface::create();
    } else if (screen == ScreenChanger::GAME) {
        WRITE_WARN("Can't start game with  this func. Use beginRunAndSlash instead.");
    } else if (screen == ScreenChanger::STORE) {
        scene = StoreInterface::create();
    } else if (screen == ScreenChanger::MAP) {
        scene = MapInterface::create();
    } else {
        WRITE_WARN("Trying to open unknown screen: " + screen);
        return;
    }
        
    changeScene(scene, screen);
}

void ScreenChanger::beginRunAndSlash(const std::string &levelId)
{
    auto scene = GameScene::create(levelId);
    changeScene(scene, ScreenChanger::GAME);
}

void ScreenChanger::showStatistics(FieldLevel::WeakPtr level, PlayerInfo::Score score, bool victory, bool complete)
{
    auto scene = StaticticsInterface::create(level, score, victory, complete);
    changeScene(scene, ScreenChanger::STATISTICS);
}
