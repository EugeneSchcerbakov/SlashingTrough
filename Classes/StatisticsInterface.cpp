//
//  StatisticsInterface.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 05/09/15.
//
//

#include "StatisticsInterface.h"

#include "PlayerInfo.h"
#include "Store.h"
#include "ScreenChanger.h"
#include "Log.h"
#include "DailyMissions.h"

StaticticsInterface* StaticticsInterface::create(FieldLevel::WeakPtr level, PlayerInfo::Score score, bool victory, bool complete)
{
    StaticticsInterface *screen = new StaticticsInterface();
    if (screen && screen->init(level, score, victory, complete)) {
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

bool StaticticsInterface::init(FieldLevel::WeakPtr level, PlayerInfo::Score score, bool victory, bool complete)
{
    if (level.expired()) {
        WRITE_ERR("Failed to init statistics screen with invalid level.");
        return false;
    }
    
    if (!cocos2d::Scene::init()) {
        return false;
    }
    
    _level = level.lock();
    
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
    _panel = cocos2d::Sprite::create("ui/ui_statistics_panel.png");
    _panel->setScale(panelScale);
    _panel->setPositionX(size.width - _panel->getContentSize().width * panelScale * 0.5f);
    _panel->setPositionY(size.height * 0.5f + panelYOffset);
    
    // texts
    auto titleText = cocos2d::ui::Text::create("Mission", "font_prototype.ttf", 23);
    titleText->setTextColor(cocos2d::Color4B::YELLOW);
    titleText->setPositionX(_panel->getContentSize().width * 0.5f);
    titleText->setPositionY(_panel->getContentSize().height - titleText->getContentSize().height * 0.5f - 5.0f);
    
    auto rewardText = cocos2d::ui::Text::create("Reward:", "font_prototype.ttf", 23);
    rewardText->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE_RIGHT);
    rewardText->setPositionX(95.0f);
    rewardText->setPositionY(_panel->getContentSize().height * 0.5f + 55.0f);
    
    auto scoreText = cocos2d::ui::Text::create("Score:", "font_prototype.ttf", 23);
    scoreText->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE_RIGHT);
    scoreText->setPositionX(95.0f);
    scoreText->setPositionY(_panel->getContentSize().height * 0.5f);
    
    int rewardValue = score.coins;
    if (complete) {
        rewardValue += _level->getCoinsReward();
    }
    
    // give coins
    PlayerInfo::getInstance().addCoins(rewardValue);
    
    auto rewardNum = cocos2d::ui::Text::create("", "font_prototype.ttf", 23);
    rewardNum->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE_LEFT);
    rewardNum->setString(cocos2d::StringUtils::format("%d", rewardValue));
    rewardNum->setPositionX(_panel->getContentSize().width * 0.5f - 5.0f);
    rewardNum->setPositionY(_panel->getContentSize().height * 0.5f + 55.0f);
    
    auto rewardIcon = cocos2d::Sprite::create("icons/icon_coin.png");
    rewardIcon->setPositionX(-rewardIcon->getContentSize().width * 0.5f - 5.0f);
    rewardIcon->setPositionY(rewardNum->getContentSize().height * 0.5f);
    rewardNum->addChild(rewardIcon);
    
    auto scoreNum = cocos2d::ui::Text::create("", "font_prototype.ttf", 23);
    scoreNum->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE_LEFT);
    scoreNum->setString(cocos2d::StringUtils::format("%d", score.score));
    scoreNum->setPositionX(_panel->getContentSize().width * 0.5f - 5.0f);
    scoreNum->setPositionY(_panel->getContentSize().height * 0.5f);
    
    _panel->addChild(titleText);
    _panel->addChild(rewardText);
    _panel->addChild(scoreText);
    _panel->addChild(rewardNum);
    _panel->addChild(scoreNum);
    
    if (victory) {
        initLootPanel();
    }
    
    PlayerInfo::getInstance().save();
    
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
    addChild(_panel, 1);
    addChild(continueText, 1);
    
    DailyMissions::getInstance().checkAfterRun();
    
    return true;
}

void StaticticsInterface::initLootPanel()
{
    std::vector<std::string> loot = _level->dropLoot();
    
    if (!loot.empty())
    {
        Store &store = Store::getInstance();
        PlayerInfo &player = PlayerInfo::getInstance();
        
        cocos2d::Vec2 xy(165.0f, 50.0f);
        
        for (const std::string &id : loot) {
            Item::WeakPtr ptr = store.getItemById(id);
            if (ptr.expired()) {continue;}
            Item::Ptr item = ptr.lock();
            
            player.Inventory.add(item);
            
            float scale = 0.6f;
            auto sprite = cocos2d::Sprite::create(item->getIcon());
            sprite->setPosition(xy);
            sprite->setScale(scale);
            _panel->addChild(sprite);
            
            xy.x += sprite->getContentSize().width * scale;
            
            DailyMissions::getInstance().statistics.incInt(id);
        }
        
        auto lootText = cocos2d::ui::Text::create("Loot:", "font_prototype.ttf", 23);
        lootText->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE_RIGHT);
        lootText->setPositionX(95.0f);
        lootText->setPositionY(_panel->getContentSize().height * 0.5f - 55.0f);
        
        _panel->addChild(lootText);
    }
}

bool StaticticsInterface::onTouch(cocos2d::Touch *touch, cocos2d::Event *e)
{
    ScreenChanger::changeScreen(ScreenChanger::MAP);
    return true;
}
