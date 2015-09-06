//
//  MissionStartPopup.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 01/09/15.
//
//

#include "MissionStartPopup.h"

#include "Store.h"
#include "ScreenChanger.h"
#include "LevelsCache.h"
#include "Log.h"

MissionStartPopup* MissionStartPopup::create(const std::string &levelId, const std::string &title)
{
    MissionStartPopup *popup = new MissionStartPopup();
    if (popup && popup->init(levelId, title)) {
        popup->autorelease();
    } else {
        delete popup;
        popup = nullptr;
    }
    return popup;
}

MissionStartPopup::MissionStartPopup()
{
}

MissionStartPopup::~MissionStartPopup()
{
}

bool MissionStartPopup::hitTest(const cocos2d::Vec2 &pt)
{
    return _play->hitTest(pt) || _panel->getBoundingBox().containsPoint(pt);
}

void MissionStartPopup::startShowEffect()
{
    if (_tint->getNumberOfRunningActions() > 0) {
        return;
    }
    
    auto director = cocos2d::Director::getInstance();
    cocos2d::Size size = director->getVisibleSize();
    
    float time = 0.2;
    float panelDeltaX = _panel->getContentSize().width * _panel->getScale() * 0.5f;
    float playDeltaX = _play->getContentSize().width * _play->getScale() * 0.5f;
    
    auto panelMove = cocos2d::MoveBy::create(time, cocos2d::Vec2(-panelDeltaX*2.0f, 0.0f));
    _panel->setPositionX(size.width + panelDeltaX);
    _panel->runAction(panelMove);
    
    auto playMove = cocos2d::MoveBy::create(time, cocos2d::Vec2(-playDeltaX*2.0f, 0.0f));
    _play->setPositionX(size.width + playDeltaX);
    _play->runAction(playMove);
    
    auto fadein = cocos2d::FadeIn::create(time);
    _tint->setOpacity(0);
    _tint->runAction(fadein);
}

void MissionStartPopup::startHideEffect(std::function<void()> func)
{
    if (_tint->getNumberOfRunningActions() > 0) {
        return;
    }
    
    float time = 0.1;
    float panelDeltaX = _panel->getContentSize().width * _panel->getScale() * 0.5f;
    float playDeltaX = _play->getContentSize().width * _play->getScale() * 0.5f;
    
    auto panelMove = cocos2d::MoveBy::create(time, cocos2d::Vec2(panelDeltaX*2.0f, 0.0f));
    _panel->runAction(panelMove);
    
    auto playMove = cocos2d::MoveBy::create(time, cocos2d::Vec2(playDeltaX*2.0f, 0.0f));
    _play->runAction(playMove);
    
    auto fadeout = cocos2d::FadeOut::create(time);
    auto call = cocos2d::CallFunc::create(func);
    auto seq = cocos2d::Sequence::create(fadeout, call, nullptr);
    _tint->runAction(seq);
}

bool MissionStartPopup::init(const std::string &levelId, const std::string &title)
{
    if (!cocos2d::ui::Layout::init()) {
        return false;
    }
    
    _level = LevelsCache::getInstance().getLevelById(levelId).lock();
    if (!_level) {
        WRITE_ERR("Failed to create MissionPopup with invalid level: " + levelId);
        return false;
    }
    
    auto director = cocos2d::Director::getInstance();

    cocos2d::Vec2 origin = director->getVisibleOrigin();
    cocos2d::Size size = director->getVisibleSize();
    
    // block touches propagation
    auto blockInput = cocos2d::EventListenerTouchOneByOne::create();
    blockInput->setSwallowTouches(true);
    blockInput->onTouchBegan = [](cocos2d::Touch* touch, cocos2d::Event* event){return true;};
    director->getEventDispatcher()->addEventListenerWithSceneGraphPriority(blockInput, this);
    
    // black underlayer
    _tint = cocos2d::Sprite::create("ui/ui_bg_tint.png");
    _tint->setScale(4.0f);
    
    // panel
    float panelScale = 1.75f;
    float panelYOffset = 180.0f;
    _panel= cocos2d::Sprite::create("ui/ui_mission_start_panel.png");
    _panel->setScale(panelScale);
    _panel->setPositionX(size.width - _panel->getContentSize().width * panelScale * 0.5f);
    _panel->setPositionY(size.height * 0.5f + panelYOffset);
    
    auto onPlay = [&](cocos2d::Ref*, cocos2d::ui::Widget::TouchEventType e) {
        if (e == cocos2d::ui::Widget::TouchEventType::ENDED) {
            startHideEffect([](){});
            ScreenChanger::beginRunAndSlash(levelId);
        }
    };
    
    // play button
    float playYOffset = -150.0f;
    _play = cocos2d::ui::Button::create("ui/ui_mission_run_button.png");
    _play->setScale(panelScale);
    _play->setPositionX(size.width - _play->getContentSize().width * panelScale * 0.5f);
    _play->setPositionY(size.height * 0.5f + playYOffset);
    _play->addTouchEventListener(onPlay);
    
    // texts
    auto titleText = cocos2d::ui::Text::create("Mission", "font_prototype.ttf", 23);
    titleText->setTextColor(cocos2d::Color4B::YELLOW);
    titleText->setPositionX(_panel->getContentSize().width * 0.5f);
    titleText->setPositionY(_panel->getContentSize().height - titleText->getContentSize().height * 0.5f - 5.0f);
    
    auto rewardText = cocos2d::ui::Text::create("Reward:", "font_prototype.ttf", 23);
    rewardText->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE_RIGHT);
    rewardText->setPositionX(95.0f);
    rewardText->setPositionY(_panel->getContentSize().height * 0.5f + 40.0f);
    
    auto rewardNum = cocos2d::ui::Text::create("", "font_prototype.ttf", 23);
    rewardNum->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE_LEFT);
    rewardNum->setString(cocos2d::StringUtils::format("%d", _level->getCoinsReward()));
    rewardNum->setPositionX(_panel->getContentSize().width * 0.5f - 5.0f);
    rewardNum->setPositionY(_panel->getContentSize().height * 0.5f + 40.0f);
    
    auto rewardIcon = cocos2d::Sprite::create("icons/icon_coin.png");
    rewardIcon->setPositionX(-rewardIcon->getContentSize().width * 0.5f - 5.0f);
    rewardIcon->setPositionY(rewardNum->getContentSize().height * 0.5f);
    rewardNum->addChild(rewardIcon);
    
    _panel->addChild(titleText);
    _panel->addChild(rewardText);
    _panel->addChild(rewardNum);
    
    initLootPanel();
    
    addChild(_tint, 0);
    addChild(_panel, 1);
    addChild(_play, 1);
    
    return true;
}

void MissionStartPopup::initLootPanel()
{
    std::vector<std::string> loot = _level->getPossibleLoot();
    
    if (!loot.empty()) {
        Store &store = Store::getInstance();
        cocos2d::Vec2 xy(165.0f, 50.0f);
    
        for (const std::string &id : loot) {
            Item::WeakPtr ptr = store.getItemById(id);
            if (ptr.expired()) {continue;}
            Item::Ptr item = ptr.lock();
        
            float scale = 0.6f;
            auto sprite = cocos2d::Sprite::create(item->getIcon());
            sprite->setPosition(xy);
            sprite->setScale(scale);
            _panel->addChild(sprite);
        
            xy.x += sprite->getContentSize().width * scale;
        }
        
        auto lootText = cocos2d::ui::Text::create("Loot:", "font_prototype.ttf", 23);
        lootText->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE_RIGHT);
        lootText->setPositionX(95.0f);
        lootText->setPositionY(_panel->getContentSize().height * 0.5f);
        
        _panel->addChild(lootText);
    }
}
