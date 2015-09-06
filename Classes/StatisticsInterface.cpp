//
//  StatisticsInterface.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 05/09/15.
//
//

#include "StatisticsInterface.h"

#include "ScreenChanger.h"
#include "Log.h"

StaticticsInterface* StaticticsInterface::create(FieldLevel::WeakPtr level, PlayerInfo::Score score, bool victory)
{
    StaticticsInterface *screen = new StaticticsInterface();
    if (screen && screen->init(level, score, victory)) {
        screen->autorelease();
    } else {
        delete screen;
        screen = nullptr;
    }
    return screen;
}

StaticticsInterface::StaticticsInterface()
{
}

StaticticsInterface::~StaticticsInterface()
{
}

bool StaticticsInterface::init(FieldLevel::WeakPtr level, PlayerInfo::Score score, bool victory)
{
    if (level.expired()) {
        WRITE_ERR("Failed to init statistics screen with invalid level.");
        return false;
    }
    
    if (!cocos2d::Scene::init()) {
        return false;
    }
    
    FieldLevel::Ptr level_ptr = level.lock();
    
    auto director = cocos2d::Director::getInstance();
    auto dispatcher = director->getEventDispatcher();
    
    cocos2d::Vec2 origin = director->getVisibleOrigin();
    cocos2d::Size size = director->getVisibleSize();
    
    auto background = cocos2d::Sprite::create("ui/ui_bg.jpeg");
    background->setAnchorPoint(cocos2d::Vec2(0.0f, 0.0f));
    background->setPosition(0.0f, 0.0f);
    background->setScale(1.5f);
    
    // panel
    float panelScale = 1.75f;
    float panelYOffset = 150.0f;
    auto panel = cocos2d::Sprite::create("ui/ui_statistics_panel.png");
    panel->setScale(panelScale);
    panel->setPositionX(size.width - panel->getContentSize().width * panelScale * 0.5f);
    panel->setPositionY(size.height * 0.5f + panelYOffset);
    
    // texts
    auto titleText = cocos2d::ui::Text::create("Mission", "font_prototype.ttf", 23);
    titleText->setTextColor(cocos2d::Color4B::YELLOW);
    titleText->setPositionX(panel->getContentSize().width * 0.5f);
    titleText->setPositionY(panel->getContentSize().height - titleText->getContentSize().height * 0.5f - 5.0f);
    
    auto rewardText = cocos2d::ui::Text::create("Reward:", "font_prototype.ttf", 23);
    rewardText->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE_RIGHT);
    rewardText->setPositionX(95.0f);
    rewardText->setPositionY(panel->getContentSize().height * 0.5f + 55.0f);
    
    auto scoreText = cocos2d::ui::Text::create("Score:", "font_prototype.ttf", 23);
    scoreText->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE_RIGHT);
    scoreText->setPositionX(95.0f);
    scoreText->setPositionY(panel->getContentSize().height * 0.5f);
    
    int rewardValue = score.coins;
    if (victory) {
        rewardValue += level_ptr->getCoinsReward();
    }
    auto rewardNum = cocos2d::ui::Text::create("", "font_prototype.ttf", 23);
    rewardNum->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE_LEFT);
    rewardNum->setString(cocos2d::StringUtils::format("%d", rewardValue));
    rewardNum->setPositionX(panel->getContentSize().width * 0.5f - 5.0f);
    rewardNum->setPositionY(panel->getContentSize().height * 0.5f + 55.0f);
    
    auto rewardIcon = cocos2d::Sprite::create("icons/icon_coin.png");
    rewardIcon->setPositionX(-rewardIcon->getContentSize().width * 0.5f - 5.0f);
    rewardIcon->setPositionY(rewardNum->getContentSize().height * 0.5f);
    rewardNum->addChild(rewardIcon);
    
    auto scoreNum = cocos2d::ui::Text::create("", "font_prototype.ttf", 23);
    scoreNum->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE_LEFT);
    scoreNum->setString(cocos2d::StringUtils::format("%d", score.score));
    scoreNum->setPositionX(panel->getContentSize().width * 0.5f - 5.0f);
    scoreNum->setPositionY(panel->getContentSize().height * 0.5f);
    
    panel->addChild(titleText);
    panel->addChild(rewardText);
    panel->addChild(scoreText);
    panel->addChild(rewardNum);
    panel->addChild(scoreNum);
    
    auto continueText = cocos2d::ui::Text::create("Tap to continue", "font_prototype.ttf", 40);
    continueText->setTextColor(cocos2d::Color4B::WHITE);
    continueText->setAnchorPoint(cocos2d::Vec2(0.5f, 0.5f));
    continueText->setPositionX(origin.x + size.width - continueText->getContentSize().width * 0.5f - 50.0f);
    continueText->setPositionY(origin.y + size.height * 0.5f - 300.0f);
    
    auto s0 = cocos2d::ScaleTo::create(0.5f, 1.1f);
    auto s1 = cocos2d::ScaleTo::create(0.5f, 1.0f);
    auto e0 = cocos2d::EaseSineInOut::create(s0);
    auto e1 = cocos2d::EaseSineInOut::create(s1);
    auto seq = cocos2d::Sequence::create(e0, e1, nullptr);
    auto eff = cocos2d::RepeatForever::create(seq);
    
    continueText->runAction(eff);
    
    auto input = cocos2d::EventListenerTouchOneByOne::create();
    input->onTouchBegan = CC_CALLBACK_2(StaticticsInterface::onTouch, this);
    dispatcher->addEventListenerWithSceneGraphPriority(input, this);
    
    addChild(background, 0);
    addChild(panel, 1);
    addChild(continueText, 1);
    
    return true;
}

bool StaticticsInterface::onTouch(cocos2d::Touch *touch, cocos2d::Event *e)
{
    ScreenChanger::changeScreen(ScreenChanger::MAP);
    return true;
}
