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
        _difficultText->setPositionX(origin.x + screen.width - _difficultText->getContentSize().width * 0.5f);
        _difficultText->setPositionY(origin.y + _difficultText->getContentSize().height * 0.5f);
    }
}

void GameInterface::setGoldPointsLabel(int value)
{
    std::string str = cocos2d::StringUtils::format("%d", value);
    _goldPointsText->setString(str);
}

void GameInterface::setKillPointsLabel(int value)
{
}

void GameInterface::setDamagePointsLabel(int value)
{
}

void GameInterface::setHealthPointsLabel(float value)
{
    value = math::clamp(value, 0.0f, 100.0f);
    _hpBar->setPercentage(value);
}

void GameInterface::setStaminaPoints(float value)
{
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
        _difficultText->setFontSize(25);
        _difficultText->setString("2");
        _difficultText->setTextHorizontalAlignment(cocos2d::TextHAlignment::CENTER);
        _difficultText->setTextVerticalAlignment(cocos2d::TextVAlignment::CENTER);
        _difficultText->setPositionX(origin.x + screen.width - _difficultText->getContentSize().width * 0.5f);
        _difficultText->setPositionY(origin.y + _difficultText->getContentSize().height * 0.5f);
        
        addChild(_timeScaleText);
        addChild(_difficultText);
    }
    
    _hpBack = cocos2d::Sprite::create("ui/ui_hp_back.png");
    _hpBack->setPositionX(origin.x + screen.width - _hpBack->getContentSize().width * 0.5f - 8.0f);
    _hpBack->setPositionY(origin.y + screen.height - _hpBack->getContentSize().height * 0.5f - 15.0f);
	_hpBack->setColor(cocos2d::Color3B(42, 119, 228));
    
    auto hpSprite = cocos2d::Sprite::create("ui/ui_hp_full.png");
    _hpBar = cocos2d::ProgressTimer::create(hpSprite);
    _hpBar->setType(cocos2d::ProgressTimer::Type::BAR);
    _hpBar->setPositionX(_hpBack->getPositionX());
    _hpBar->setPositionY(_hpBack->getPositionY() + 1.0f);
    _hpBar->setBarChangeRate(cocos2d::Vec2(1.0f, 0.0f));
    _hpBar->setMidpoint(cocos2d::Vec2(0.0f, 0.5f));
    _hpBar->setPercentage(100.0f);
	_hpBar->setColor(cocos2d::Color3B(42, 119, 228));
    
    addChild(_hpBar);
    addChild(_hpBack);
    
    _coinsIcon = cocos2d::Sprite::create("icons/icon_coin.png");
    _coinsIcon->setScale(1.8f);
    _coinsIcon->setPositionX(-_coinsIcon->getContentSize().width * _coinsIcon->getScale() * 0.5f);
    _coinsIcon->setPositionY(_coinsIcon->getContentSize().height);
    
    _goldPointsText = cocos2d::ui::Text::create("", "font_prototype.ttf", 53);
    _goldPointsText->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE_LEFT);
    _goldPointsText->setPosition(cocos2d::Vec2(116.0f, 28.0f));
    _goldPointsText->setTextColor(cocos2d::Color4B::YELLOW);
    _goldPointsText->setPositionY(screen.height - 60.0f);
    _goldPointsText->addChild(_coinsIcon);
    
    addChild(_goldPointsText);
    scheduleUpdate();
    
    return true;
}

void GameInterface::update(float dt)
{
    cocos2d::Director *director;
    director = cocos2d::Director::getInstance();
    
    cocos2d::Vec2 origin = director->getVisibleOrigin();
    cocos2d::Size screen = director->getVisibleSize();
    
    float frame_half = screen.width * 0.5f;
    float label_half = _goldPointsText->getContentSize().width * 0.5f;
    float coins_half = _coinsIcon->getContentSize().width * _coinsIcon->getScale() * 0.5f;
    
    _goldPointsText->setPositionX(frame_half - label_half + coins_half);
    
    if (_hpBar->getPercentage() < 30.0f && _hpBack->getNumberOfRunningActions() <= 0)
    {
        _hpBack->runAction(cretePulsationWithColor());
        _hpBar->runAction(cretePulsationWithColor());
    }
    if (_hpBar->getPercentage() > 30.0 && _hpBack->getNumberOfRunningActions() > 0)
    {
        _hpBack->stopAllActions();
        _hpBar->stopAllActions();
        
        _hpBack->setScale(1.0f);
        _hpBar->setScale(1.0f);
        
        _hpBack->setColor(cocos2d::Color3B(42, 119, 228));
        _hpBar->setColor(cocos2d::Color3B(42, 119, 228));
    }
}

cocos2d::FiniteTimeAction* GameInterface::cretePulsationWithColor() const
{
    auto scale0 = cocos2d::ScaleTo::create(0.5f, 1.2f);
    auto scale1 = cocos2d::ScaleTo::create(0.5f, 1.0f);
    auto scaleEase1 = cocos2d::EaseSineIn::create(scale0);
    auto scaleEase2 = cocos2d::EaseSineOut::create(scale1);
    auto scaleSeq = cocos2d::Sequence::create(scaleEase1, scaleEase2, nullptr);
    
    auto tint0 = cocos2d::TintTo::create(0.5f, 255, 0, 0);
	auto tint1 = cocos2d::TintTo::create(0.5f, 42, 119, 228);
    auto tintEase0 = cocos2d::EaseSineIn::create(tint0);
    auto tintEase1 = cocos2d::EaseSineOut::create(tint1);
    auto tintSeq = cocos2d::Sequence::create(tintEase0, tintEase1, nullptr);

    auto both = cocos2d::Spawn::create(scaleSeq, tintSeq, nullptr);
    auto effect = cocos2d::RepeatForever::create(both);
    
    return effect;
}
