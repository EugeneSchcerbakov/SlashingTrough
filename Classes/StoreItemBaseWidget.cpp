//
//  StoreItemBaseWidget.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 02/08/15.
//
//

#include "StoreItemBaseWidget.h"

#include "PlayerInfo.h"

StoreItemBaseWidget::StoreItemBaseWidget(Item::WeakPtr item)
: _item(item)
{
}

StoreItemBaseWidget::~StoreItemBaseWidget()
{
}

bool StoreItemBaseWidget::init()
{
    if (!cocos2d::ui::Layout::init()) {
        return false;
    }
    
    Item::Ptr item = _item.lock();
    
    setBackGroundImageScale9Enabled(false);
    setBackGroundImage("ui/ui_shop_item_panel.png");
    setContentSize(getBackGroundImageTextureSize());
    setAnchorPoint(cocos2d::Vec2(0.5f, 0.5f));
    
    const cocos2d::Size iconArea(183.0f, 174.0f);
    
    float iconScale = 1.1f;
    auto *itemIcon = cocos2d::Sprite::create(item->getIcon());
    itemIcon->setScale(iconScale);
    itemIcon->setPositionX(iconArea.width * 0.5f);
    itemIcon->setPositionY(getContentSize().height - iconArea.height * 0.5f);
    
    auto itemName = cocos2d::ui::Text::create();
    itemName->setFontName("font_prototype.ttf");
    itemName->setFontSize(43);
    itemName->setTextColor(cocos2d::Color4B(240, 193, 51, 255));
    itemName->setString(item->getName());
    itemName->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE_LEFT);
    itemName->setPositionX(getContentSize().width * 0.5f - 110.0f);
    itemName->setPositionY(getContentSize().height - itemName->getContentSize().height * 0.5f - 13.0f);
    
    float buttonScale = 1.8f;
    _button = StoreItemButton::create();
    _button->setScale(buttonScale);
    _button->setPositionX(getContentSize().width - _button->getContentSize().width * buttonScale * 0.5f - 15.0f);
    _button->setPositionY(_button->getContentSize().height * buttonScale * 0.5f + 15.0f);
    _button->addTouchEventListener(CC_CALLBACK_2(StoreItemBaseWidget::onBuyPressed, this));
    _button->setPrice(item->getPrice());
    _button->switchState(StoreItemButton::State::BUY);
    
    scheduleUpdate();
    addChild(_button, 0);
    addChild(itemName, 0);
    addChild(itemIcon, 0);
    
    return true;
}

void StoreItemBaseWidget::update(float dt)
{
    Item::Ptr ptr = _item.lock();
    PlayerInfo &player = PlayerInfo::getInstance();
    
    if (player.Inventory.owned(ptr) && !player.equipped(ptr) && !_button->isState(StoreItemButton::State::EQUIP))
    {
        _button->switchState(StoreItemButton::State::EQUIP);
    }
    if (player.equipped(ptr) && !_button->isState(StoreItemButton::State::EQUIPPED))
    {
        _button->switchState(StoreItemButton::State::EQUIPPED);
    }
}

void StoreItemBaseWidget::onBuy()
{
    Item::Ptr ptr = _item.lock();
    PlayerInfo &player = PlayerInfo::getInstance();

    bool needSave = false;
    if (!player.Inventory.owned(ptr)) {
        if (Store::getInstance().buy(ptr->getId())) {
            player.equip(ptr);
            needSave = true;
        }
    } else if (!player.equipped(ptr)) {
        player.equip(ptr);
        needSave = true;
    }

    if (needSave) {
        player.save();
    }
}

void StoreItemBaseWidget::onBuyPressed(cocos2d::Ref *sender, cocos2d::ui::Widget::TouchEventType event)
{
    if (event == cocos2d::ui::Widget::TouchEventType::ENDED)
    {
        onBuy();
    }
}
