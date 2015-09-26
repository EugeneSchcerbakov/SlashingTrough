//
//  StoreCystallWidget.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 18/09/15.
//
//

#include "StoreCystallWidget.h"

#include "PlayerInfo.h"
#include "Log.h"

StoreCrystallWidget* StoreCrystallWidget::create(Item::WeakPtr item)
{
    StoreCrystallWidget *widget = new StoreCrystallWidget(item);
    if (widget && widget->init()) {
        widget->autorelease();
    } else {
        delete widget;
        widget = nullptr;
    }
    return widget;
}

StoreCrystallWidget::StoreCrystallWidget(Item::WeakPtr item)
: StoreItemBaseWidget(item)
{
}

StoreCrystallWidget::~StoreCrystallWidget()
{
}

bool StoreCrystallWidget::init()
{
    if (!cocos2d::ui::Layout::init()) {
        return false;
    }
    
    if (_item.expired()) {
        WRITE_ERR("Failed to craete crystall store widget.");
        return false;
    }
    
    Crystall *crystall = Crystall::cast(_item.lock());
    
    crystall->refreshState();
    
    if (!crystall) {
        WRITE_ERR("Failed to create crystall store widget");
        return false;
    }
    
    setBackGroundImageScale9Enabled(false);
    setBackGroundImage("ui/ui_shop_item_panel_crystall.png");
    setContentSize(getBackGroundImageTextureSize());
    setAnchorPoint(cocos2d::Vec2(0.5f, 0.5f));
    
    float buttonScale = 1.85f;
    _button = StoreItemButton::create();
    _button->setScale(buttonScale);
    _button->setPositionX(_button->getContentSize().width * buttonScale * 0.5f - 3.0f);
    _button->setPositionY(_button->getContentSize().height * buttonScale * 0.5f - 5.0f);
    _button->addTouchEventListener(CC_CALLBACK_2(StoreCrystallWidget::onBuyPressed, this));
    _button->switchState(StoreItemButton::State::EQUIP);
    _button->setVisible(crystall->isUnlocked());
    
    const cocos2d::Size iconArea(183.0f, 174.0f);
    
    auto itemIcon = cocos2d::Sprite::create(crystall->getIcon());
    itemIcon->setPositionX(iconArea.width * 0.5f);
    itemIcon->setPositionY(getContentSize().height - iconArea.height * 0.5f);
    
    auto kindIcon = cocos2d::Sprite::create();
    if (crystall->isKind(Crystall::Kind::WEAPON)) {
        kindIcon->setTexture("icons/icon_wpn_type.png");
    } else if (crystall->isKind(Crystall::Kind::ARMOR)) {
        kindIcon->setTexture("icons/icon_arm_type.png");
    } else {
        WRITE_WARN("Unknown crystall kind.");
    }
    kindIcon->setAnchorPoint(cocos2d::Vec2::ANCHOR_TOP_RIGHT);
    kindIcon->setPositionX(iconArea.width - 5.0f);
    kindIcon->setPositionY(getContentSize().height - 5.0f);
    
    auto itemName = cocos2d::ui::Text::create(crystall->getName(), "font_prototype.ttf", 43);
    itemName->setTextColor(cocos2d::Color4B(240, 193, 51, 255));
    itemName->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE_LEFT);
    itemName->setPositionX(iconArea.width + 10.0f);
    itemName->setPositionY(getContentSize().height - itemName->getContentSize().height * 0.5f - 2.0f);
    
    auto itemDesc = cocos2d::ui::Text::create(crystall->getDesc(), "font_prototype.ttf", 25);
    itemDesc->setTextColor(cocos2d::Color4B::WHITE);
    itemDesc->setTextAreaSize(cocos2d::Size(424.0f, 121.0f));
    itemDesc->ignoreContentAdaptWithSize(false);
    itemDesc->setAnchorPoint(cocos2d::Vec2::ANCHOR_TOP_LEFT);
    itemDesc->setPositionX(iconArea.width + 10);
    itemDesc->setPositionY(getContentSize().height - 52.0f);
    
    initShardsPanel();
    
    addChild(itemIcon);
    addChild(kindIcon);
    addChild(itemName);
    addChild(itemDesc);
    addChild(_button);
    scheduleUpdate();
    
    return true;
}

void StoreCrystallWidget::initShardsPanel()
{
    Crystall *crystall = Crystall::cast(_item.lock());
    
    bool lastTier = crystall->getTier() >= crystall->getMaxTier() - 1;
    int shardsTotal = crystall->getCurrentTierData().shards;
    int shardsFound = lastTier ? shardsTotal : crystall->getShardsToNextTier();
    
    std::string label = cocos2d::StringUtils::format("%d/%d", shardsFound, shardsTotal);
    
    auto text = cocos2d::ui::Text::create(label, "font_prototype.ttf", 48.0f);
    text->setTextColor(cocos2d::Color4B(76, 17, 49, 255));
    text->setAnchorPoint(cocos2d::Vec2::ANCHOR_BOTTOM_RIGHT);
    text->setPositionX(getContentSize().width - 10.0f);
    text->setPositionY(3.0f);
    
    // create shards bar
    
    cocos2d::Node *shardsBar = cocos2d::Node::create();
    cocos2d::Size shardsBarSize(0.0f, 0.0f);
    cocos2d::Vec2 xy(0.0f, 0.0f);
    
    for (int k = 0; k < shardsTotal; k++)
    {
        std::string texture = k < shardsFound ?
        "ui/ui_bar_shard_found.png" :
        "ui/ui_bar_shard_notfound.png";
        
        auto shard = cocos2d::Sprite::create(texture);
        shard->setPosition(xy);
        
        int z = std::max(0, (shardsTotal - k) - 1);
        float w = shard->getContentSize().width;
        float h = shard->getContentSize().height;
        
        xy.x -= w * 0.5f;
        shardsBarSize.width += w * 0.5f;
        if (h > shardsBarSize.height) {
            shardsBarSize.height = h;
        }
        
        shardsBar->addChild(shard, z);
    }
    
    shardsBar->setContentSize(shardsBarSize);
    shardsBar->setPositionX(getContentSize().width - 175.0f);
    shardsBar->setPositionY(shardsBarSize.height * 0.5f + 5.0f);
    
    addChild(text);
    addChild(shardsBar);
}

void StoreCrystallWidget::update(float dt)
{
    Crystall::Ptr item = _item.lock();
    Crystall *crystall = Crystall::cast(item);
    
    if (crystall->isUnlocked())
    {
        PlayerInfo &player = PlayerInfo::getInstance();
        
        StoreItemButton::State state;
        state = player.equipped(item)
        ? StoreItemButton::State::EQUIPPED
        : StoreItemButton::State::EQUIP;
        
        _button->switchState(state);
    }
}

void StoreCrystallWidget::onBuyPressed(cocos2d::Ref *sender, cocos2d::ui::Widget::TouchEventType event)
{
    if (event == cocos2d::ui::Widget::TouchEventType::ENDED)
    {
        Crystall::Ptr item = _item.lock();
        Crystall *crystall = Crystall::cast(item);
    
        if (crystall->isUnlocked())
        {
            PlayerInfo &player = PlayerInfo::getInstance();
        
            if (!player.equipped(item)) {
                player.equip(item);
                player.save();
            }
        }
    }
}

