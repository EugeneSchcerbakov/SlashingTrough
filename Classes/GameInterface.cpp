//
//  GameInterface.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 11/04/15.
//
//

#include "GameInterface.h"

#include "GameInfo.h"
#include "Utils.h"

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

void GameInterface::setTimeScaleLabel(float timeScale)
{
    if (misc::isPlatformDesctop()) {
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
}

void GameInterface::setDifficultLable(const std::string &text)
{
    if (misc::isPlatformDesctop()) {
        cocos2d::Director *director;
        director = cocos2d::Director::getInstance();
    
        cocos2d::Vec2 origin = director->getVisibleOrigin();
        cocos2d::Size screen = director->getVisibleSize();
        _difficultText->setString(text);
        _difficultText->setPositionX(origin.x + _difficultText->getContentSize().width * 0.5f);
        _difficultText->setPositionY(screen.height - _difficultText->getContentSize().height * 0.5f);
    }
}

void GameInterface::setGoldPointsLabel(int value)
{
    std::string str = cocos2d::StringUtils::format("%d", value);
    _goldPointsText->setString(str);
}

void GameInterface::setKillPointsLabel(int value)
{
    std::string str = cocos2d::StringUtils::format("%d", value);
    _killPointsText->setString(str);
}

void GameInterface::setDamagePointsLabel(int value)
{
    std::string str = cocos2d::StringUtils::format("%d", value);
    _damagePointsText->setString(str);
}

void GameInterface::setHealthPointsLabel(int value)
{
    std::string str = cocos2d::StringUtils::format("%d%", value);
    _healthPointsText->setString(str);
}

void GameInterface::setStaminaPoints(float value)
{
    _staminaBar->setProgress(value);
}

bool GameInterface::init()
{
    if (!cocos2d::Layer::init()) {
        return false;
    }
    
    cocos2d::Director *director;
    director = cocos2d::Director::getInstance();
    
    cocos2d::Vec2 origin = director->getVisibleOrigin();
    cocos2d::Size screen = director->getVisibleSize();
    
    if (misc::isPlatformDesctop())
    {
        _timeScaleText = cocos2d::ui::Text::create();
        _timeScaleText->setFontName("font_prototype.ttf");
        _timeScaleText->setFontSize(150);
        _timeScaleText->setString("");
        _timeScaleText->setTextHorizontalAlignment(cocos2d::TextHAlignment::CENTER);
        _timeScaleText->setTextVerticalAlignment(cocos2d::TextVAlignment::CENTER);
        const float timeScaleTextShift = -400.0f;
        _timeScaleText->setPositionX(origin.x + screen.width * 0.5f);
        _timeScaleText->setPositionY(origin.y + screen.height * 0.5f + timeScaleTextShift);
        _timeScaleText->setVisible(false);
        
        _difficultText = cocos2d::ui::Text::create();
        _difficultText->setFontName("arial");
        _difficultText->setFontSize(30);
        _difficultText->setString("2");
        _difficultText->setTextHorizontalAlignment(cocos2d::TextHAlignment::CENTER);
        _difficultText->setTextVerticalAlignment(cocos2d::TextVAlignment::CENTER);
        _difficultText->setPositionX(origin.x + _difficultText->getContentSize().width * 0.5f);
        _difficultText->setPositionY(screen.height - _difficultText->getContentSize().height * 0.5f);
        
        addChild(_timeScaleText);
        addChild(_difficultText);
    }
    
    cocos2d::Color4F backingColor(0.2f, 0.2f, 0.2f, 0.7f);
    
    _goldPointsText = cocos2d::ui::Text::create();
    _goldPointsText->setFontName("font_prototype.ttf");
    _goldPointsText->setFontSize(36);
    _goldPointsText->setTextHorizontalAlignment(cocos2d::TextHAlignment::CENTER);
    _goldPointsText->setTextVerticalAlignment(cocos2d::TextVAlignment::CENTER);
    _goldPointsText->setPosition(cocos2d::Vec2(116.0f, 28.0f));
    _goldPointsText->setTextColor(cocos2d::Color4B::YELLOW);
    
    _killPointsText = cocos2d::ui::Text::create();
    _killPointsText->setFontName("font_prototype.ttf");
    _killPointsText->setFontSize(36);
    _killPointsText->setTextHorizontalAlignment(cocos2d::TextHAlignment::CENTER);
    _killPointsText->setTextVerticalAlignment(cocos2d::TextVAlignment::CENTER);
    _killPointsText->setPosition(cocos2d::Vec2(90.0f, 28.0f));
    
    _damagePointsText = cocos2d::ui::Text::create();
    _damagePointsText->setFontName("font_prototype.ttf");
    _damagePointsText->setFontSize(36);
    _damagePointsText->setTextHorizontalAlignment(cocos2d::TextHAlignment::CENTER);
    _damagePointsText->setTextVerticalAlignment(cocos2d::TextVAlignment::CENTER);
    _damagePointsText->setPosition(cocos2d::Vec2(90.0f, 28.0f));
    
    _healthPointsText = cocos2d::ui::Text::create();
    _healthPointsText->setFontName("font_prototype.ttf");
    _healthPointsText->setFontSize(36);
    _healthPointsText->setTextHorizontalAlignment(cocos2d::TextHAlignment::CENTER);
    _healthPointsText->setTextVerticalAlignment(cocos2d::TextVAlignment::CENTER);
    _healthPointsText->setPosition(cocos2d::Vec2(105.0f, 28.0f));
    _healthPointsText->setTextColor(cocos2d::Color4B::RED);

    cocos2d::Sprite *goldIcon = cocos2d::Sprite::create("icons/icon_coin.png");
    cocos2d::Sprite *killIcon = cocos2d::Sprite::create("icons/icon_kills.png");
    cocos2d::Sprite *damageIcon = cocos2d::Sprite::create("icons/icon_dmg.png");
    cocos2d::Sprite *healthIcon = cocos2d::Sprite::create("icons/icon_health.png");
    goldIcon->setPosition(27.0f, 28.0f);
    goldIcon->setScale(1.3f);
    killIcon->setPosition(27.0f, 28.0f);
    killIcon->setScale(1.3f);
    damageIcon->setPosition(27.0f, 28.0f);
    damageIcon->setScale(1.3f);
    healthIcon->setPosition(27.0f, 28.0f);
    
    cocos2d::Size goldPointsBackSize(190.0f, 60.0f);
    cocos2d::DrawNode *goldPointsBack = cocos2d::DrawNode::create();
    goldPointsBack->drawSolidRect(cocos2d::Vec2(0.0f, 0.0f), cocos2d::Vec2(goldPointsBackSize), backingColor);
    goldPointsBack->setPositionX(screen.width - goldPointsBackSize.width - 10.0f);
    goldPointsBack->setPositionY(screen.height - goldPointsBackSize.height - 10.0f);
    goldPointsBack->addChild(_goldPointsText);
    goldPointsBack->addChild(goldIcon);
    
    cocos2d::Size killPointsBackSize(160.0f, 60.0f);
    cocos2d::DrawNode *killPointsBack = cocos2d::DrawNode::create();
    killPointsBack->drawSolidRect(cocos2d::Vec2(0.0f, 0.0f), cocos2d::Vec2(killPointsBackSize), backingColor);
    killPointsBack->setPositionX(screen.width - killPointsBackSize.width - 10.0f);
    killPointsBack->setPositionY(goldPointsBack->getPositionY() - goldPointsBackSize.height - 10.0f);
    killPointsBack->addChild(killIcon);
    killPointsBack->addChild(_killPointsText);
    
    cocos2d::Size damagePointsBackSize(160.0f, 60.0f);
    cocos2d::DrawNode *damagePointsBack = cocos2d::DrawNode::create();
    damagePointsBack->drawSolidRect(cocos2d::Vec2(0.0f, 0.0f), cocos2d::Vec2(damagePointsBackSize), backingColor);
    damagePointsBack->setPositionX(screen.width - killPointsBackSize.width - 10.0f);
    damagePointsBack->setPositionY(killPointsBack->getPositionY() - damagePointsBackSize.height - 10.0f);
    damagePointsBack->addChild(damageIcon);
    damagePointsBack->addChild(_damagePointsText);
    
    cocos2d::Size healthPointsBackSize(160.0f, 60.0f);
    cocos2d::DrawNode *healthPointsBack = cocos2d::DrawNode::create();
    healthPointsBack->drawSolidRect(cocos2d::Vec2(0.0f, 0.0f), cocos2d::Vec2(healthPointsBackSize), backingColor);
    healthPointsBack->setPositionX(screen.width - killPointsBackSize.width - 10.0f);
    healthPointsBack->setPositionY(damagePointsBack->getPositionY() - healthPointsBackSize.height - 10.0f);
    healthPointsBack->addChild(healthIcon);
    healthPointsBack->addChild(_healthPointsText);
    
    _staminaBar = StaminaBarWidget::create();
    _staminaBar->setPositionX(screen.width * 0.5f);
    _staminaBar->setPositionY(_staminaBar->getContentSize().height * 0.5f + 15.0f);
    _staminaBar->setProgress(1.0f);
    
    addChild(_staminaBar);
    addChild(goldPointsBack);
    addChild(killPointsBack);
    addChild(damagePointsBack);
    addChild(healthPointsBack);
    
    GameInfo &gameinfo = GameInfo::getInstance();
    
    goldPointsBack->setVisible(gameinfo.getConstBool("GOLD_POINTS_VISIBILITY"));
    killPointsBack->setVisible(gameinfo.getConstBool("KILL_POINTS_VISIBILITY"));
    damagePointsBack->setVisible(gameinfo.getConstBool("DAMAGE_POINTS_VISIBILITY"));
    healthPointsBack->setVisible(gameinfo.getConstBool("HEALTH_POINTS_VISIBILITY"));
    _staminaBar->setVisible(gameinfo.getConstBool("STAMINA_BAR_VISIBILITY"));
    
    return true;
}
