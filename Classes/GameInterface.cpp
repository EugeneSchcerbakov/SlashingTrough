//
//  GameInterface.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 11/04/15.
//
//

#include "GameInterface.h"

GameInterface* GameInterface::create()
{
    GameInterface *gameInterface = new GameInterface();
	if (gameInterface && gameInterface->init()) {
		gameInterface->autorelease();
    } else {
		delete gameInterface;
		gameInterface = nullptr;
    }
	return gameInterface;
}

GameInterface::GameInterface()
{
}

GameInterface::~GameInterface()
{
}

void GameInterface::SetTimeScaleLabel(float timeScale)
{
    std::string label = cocos2d::StringUtils::format("x%.1f", timeScale);
    
    _timeScaleText->setVisible(true);
    _timeScaleText->setString(label);
    _timeScaleText->stopAllActions();
    
    if (timeScale == 1.0f)
    {
        std::function<void()> func = [&]() {
            _timeScaleText->setVisible(false);
        };
        
        cocos2d::DelayTime *delay = cocos2d::DelayTime::create(2.0f);
        cocos2d::CallFunc *callback = cocos2d::CallFunc::create(func);
        cocos2d::Sequence *seq = cocos2d::Sequence::create(delay, callback, nullptr);
        _timeScaleText->runAction(seq);
    }
}

bool GameInterface::init()
{
    if (!cocos2d::Layer::init()) {
        return false;
    }
    
    cocos2d::Director *director;
    director = cocos2d::Director::getInstance();
    
    cocos2d::Vec2 origin = director->getVisibleOrigin();
    cocos2d::Vec2 size = director->getVisibleSize();
    
    _timeScaleText = cocos2d::ui::Text::create();
    _timeScaleText->setFontName("font_prototype.ttf");
    _timeScaleText->setFontSize(150);
    _timeScaleText->setString("");
    _timeScaleText->setTextHorizontalAlignment(cocos2d::TextHAlignment::CENTER);
    _timeScaleText->setTextVerticalAlignment(cocos2d::TextVAlignment::CENTER);
    const float timeScaleTextShift = -400.0f;
    _timeScaleText->setPositionX(origin.x + size.x * 0.5f);
    _timeScaleText->setPositionY(origin.y + size.y * 0.5f + timeScaleTextShift);
    _timeScaleText->setVisible(false);
    
    addChild(_timeScaleText);
    
    return true;
}
