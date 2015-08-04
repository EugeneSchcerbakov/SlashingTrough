//
//  StoreItemBaseWidget.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 02/08/15.
//
//

#include "StoreItemBaseWidget.h"

#include "SessionInfo.h"

StoreItemBaseWidget::StoreItemBaseWidget(Equip::WeakPtr item)
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
    
    Equip::Ptr item = _item.lock();
    
    setBackGroundImageScale9Enabled(false);
    setBackGroundImage("ui/ui_shop_item-plate.png");
    setContentSize(getBackGroundImageTextureSize());
    setAnchorPoint(cocos2d::Vec2(0.5f, 0.5f));
    
    auto *itemIcon = cocos2d::Sprite::create(item->icon);
    itemIcon->setScale(1.4f);
    
    auto itemName = cocos2d::ui::Text::create();
    itemName->setFontName("font_prototype.ttf");
    itemName->setFontSize(27);
    itemName->setTextColor(cocos2d::Color4B::BLACK);
    itemName->setTextHorizontalAlignment(cocos2d::TextHAlignment::CENTER);
    itemName->setTextVerticalAlignment(cocos2d::TextVAlignment::CENTER);
    itemName->setString(item->name);
    
    auto *iconPanel = cocos2d::ui::Layout::create();
    iconPanel->setBackGroundImage("ui/ui_shop_item-plate_item-icon_background.png");
    iconPanel->setContentSize(iconPanel->getBackGroundImageTextureSize());
    iconPanel->setPositionX(iconPanel->getContentSize().width * 0.5f);
    iconPanel->setPositionY(iconPanel->getContentSize().height * 0.5f);
    iconPanel->setAnchorPoint(cocos2d::Vec2(0.5f, 0.5f));
    iconPanel->addChild(itemIcon);
    iconPanel->addChild(itemName);
    
    itemIcon->setPositionX(iconPanel->getContentSize().width * 0.5f);
    itemIcon->setPositionY(iconPanel->getContentSize().height * 0.5f + 20.0f);
    itemName->setPositionX(iconPanel->getContentSize().width * 0.5f);
    itemName->setPositionY(40.0f);
    
    auto desc = cocos2d::ui::Text::create();
    desc->setFontName("font_prototype.ttf");
    desc->setFontSize(25);
    desc->setTextColor(cocos2d::Color4B(150, 87, 38, 255));
    desc->setTextHorizontalAlignment(cocos2d::TextHAlignment::LEFT);
    desc->setTextVerticalAlignment(cocos2d::TextVAlignment::TOP);
    desc->setString(item->desc);
    desc->setPositionX(getContentSize().width * 0.5f - 45.0f);
    desc->setPositionY(115.0f);
    desc->setAnchorPoint(cocos2d::Vec2(0.0f, 0.5f));
    
    _button = StoreItemButton::create();
    _button->setPositionX(getContentSize().width - _button->getContentSize().width * 0.5f);
    _button->setPositionY(_button->getContentSize().height * 0.5f);
    _button->addTouchEventListener(CC_CALLBACK_2(StoreItemBaseWidget::onBuyPressed, this));
    _button->setPrice(item->price);
    _button->switchState(StoreItemButton::State::BUY);
    
    scheduleUpdate();
    addChild(_button, 0);
    addChild(iconPanel, 1);
    addChild(desc, 2);
    
    return true;
}

void StoreItemBaseWidget::update(float dt)
{
    Equip::Ptr ptr = _item.lock();
    SessionInfo &save = SessionInfo::getInstance();
    
    if (save.isEquipOwned(ptr) && !save.isEquipped(ptr) && !_button->isState(StoreItemButton::State::EQUIP))
    {
        _button->switchState(StoreItemButton::State::EQUIP);
    }
    if (save.isEquipped(ptr) && !_button->isState(StoreItemButton::State::EQUIPPED))
    {
        _button->switchState(StoreItemButton::State::EQUIPPED);
    }
}

void StoreItemBaseWidget::onBuyPressed(cocos2d::Ref *sender, cocos2d::ui::Widget::TouchEventType event)
{
    if (event == cocos2d::ui::Widget::TouchEventType::ENDED)
    {
        Equip::Ptr ptr = _item.lock();
        SessionInfo &profile = SessionInfo::getInstance();
        
        bool needSave = false;
        if (!profile.isEquipOwned(ptr)) {
            if (Store::getInstance().buy(ptr->id)) {
                profile.equip(ptr);
                needSave = true;
            }
        } else if (!profile.isEquipped(ptr)) {
            profile.equip(ptr);
            needSave = true;
        }
        
        if (needSave) {
            profile.save();
        }
    }
}
