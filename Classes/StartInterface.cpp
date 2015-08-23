//
//  StartScreen.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 23/08/15.
//
//

#include "StartInterface.h"

StartInterface* StartInterface::create()
{
    StartInterface *startscreen = new StartInterface();
    if (startscreen && startscreen->init()) {
        startscreen->autorelease();
    } else {
        delete startscreen;
        startscreen = nullptr;
    }
    return startscreen;
}

StartInterface::StartInterface()
{
}

StartInterface::~StartInterface()
{
}

bool StartInterface::init()
{
    if (!cocos2d::Scene::init()) {
        return false;
    }
    
    auto director = cocos2d::Director::getInstance();
    auto dispatcher = director->getEventDispatcher();
    
    cocos2d::Vec2 origin = director->getVisibleOrigin();
    cocos2d::Size size = director->getVisibleSize();
    
    auto background = cocos2d::Sprite::create("ui/ui_bg.jpeg");
    background->setAnchorPoint(cocos2d::Vec2(0.0f, 0.0f));
    background->setPosition(0.0f, 0.0f);
    background->setScale(1.5f);
    
    float knighScale = 1.8f;
    float knightYOffset = -80.0f;
    auto knight = cocos2d::Sprite::create("ui/ui_startscreen_knight.png");
    knight->setScale(knighScale);
    knight->setPositionX(origin.x + size.width * 0.5f);
    knight->setPositionY(origin.y + size.height - knight->getContentSize().height * knighScale * 0.5f + knightYOffset);
    
    float settingsScale = 2.0f;
    auto settings = cocos2d::ui::Button::create("ui/ui_btn_options.png");
    settings->setScale(settingsScale);
    settings->setPositionX(origin.x + settings->getContentSize().width * settingsScale * 0.5f + 20.0f);
    settings->setPositionY(origin.y + size.height - settings->getContentSize().height * settingsScale * 0.5f - 20.0f);
    
    float titleYOffset = -200.0f;
    auto titleText = cocos2d::ui::Text::create("Slashing Trough", "font_prototype.ttf", 70);
    titleText->setTextHorizontalAlignment(cocos2d::TextHAlignment::CENTER);
    titleText->setTextVerticalAlignment(cocos2d::TextVAlignment::CENTER);
    titleText->setTextColor(cocos2d::Color4B::WHITE);
    titleText->setPositionX(origin.x + size.width * 0.5f);
    titleText->setPositionY(origin.y + size.height * 0.5f + titleYOffset);
    
    auto continueText = cocos2d::ui::Text::create("Tap to continue", "font_prototype.ttf", 40);
    continueText->setTextHorizontalAlignment(cocos2d::TextHAlignment::CENTER);
    continueText->setTextVerticalAlignment(cocos2d::TextVAlignment::CENTER);
    continueText->setTextColor(cocos2d::Color4B::WHITE);
    continueText->setAnchorPoint(cocos2d::Vec2(0.5f, 0.5f));
    continueText->setPositionX(titleText->getPositionX() +
                               titleText->getContentSize().width * 0.5f - continueText->getContentSize().width * 0.5f);
    continueText->setPositionY(origin.y + size.height * 0.5f - 300.0f);
    
    auto s0 = cocos2d::ScaleTo::create(0.5f, 1.1f);
    auto s1 = cocos2d::ScaleTo::create(0.5f, 1.0f);
    auto e0 = cocos2d::EaseSineInOut::create(s0);
    auto e1 = cocos2d::EaseSineInOut::create(s1);
    auto seq = cocos2d::Sequence::create(e0, e1, nullptr);
    auto eff = cocos2d::RepeatForever::create(seq);
    
    continueText->runAction(eff);
    
    auto input = cocos2d::EventListenerTouchOneByOne::create();
    input->onTouchBegan = CC_CALLBACK_2(StartInterface::onTouch, this);
    dispatcher->addEventListenerWithSceneGraphPriority(input, this);
    
    addChild(background, 0);
    addChild(knight, 1);
    addChild(settings, 1);
    addChild(titleText, 2);
    addChild(continueText, 2);
    
    return true;
}

bool StartInterface::onTouch(cocos2d::Touch *touch, cocos2d::Event *e)
{
    auto director = cocos2d::Director::getInstance();
    auto dispatcher = director->getEventDispatcher();
    dispatcher->dispatchCustomEvent("StartButtonPressed");
    
    return true;
}
