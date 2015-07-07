//
//  StoreItemButton.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 06/06/15.
//
//

#include "StoreItemButton.h"

StoreItemButton* StoreItemButton::create()
{
    StoreItemButton *button = new StoreItemButton();
    if (button && button->init()) {
        button->autorelease();
    } else {
        delete button;
        button = nullptr;
    }
    return button;
}

StoreItemButton::StoreItemButton()
: _state(State::BUY)
, _price(0)
{
}

StoreItemButton::~StoreItemButton()
{
}

void StoreItemButton::switchState(State state)
{
    if (state == State::BUY) {
        _state = state;
        InitBuyState();
    } else if (state == State::EQUIP) {
        _state = state;
        removeAllChildrenWithCleanup(true);
        loadTextures("ui/ui_shop_item-plate_btn_equip.png", "ui/ui_shop_item-plate_btn_equip_pressed.png");
    } else if (state == State::EQUIPPED) {
        _state = state;
        InitEquippedState();
    } else {
        CC_ASSERT(false);
    }
}

void StoreItemButton::setPrice(int price)
{
    _price = price;
}

bool StoreItemButton::isState(State state) const
{
    return _state == state;
}

bool StoreItemButton::init()
{
    if (!cocos2d::ui::Button::init()) {
        return false;
    }
    
    switchState(_state);
    
    return true;
}

void StoreItemButton::InitBuyState()
{
    removeAllChildrenWithCleanup(true);
    loadTextures("ui/ui_shop_item-plate_btn_buy.png", "ui/ui_shop_item-plate_btn_buy_pressed.png");
    
    auto coin = cocos2d::Sprite::create("icons/icon_coin.png");
    coin->setScale(1.5f);
    coin->setPositionX(getContentSize().width * 0.5f - 25.0f);
    coin->setPositionY(getContentSize().height * 0.5f);
    
    auto price = cocos2d::ui::Text::create();
    price->setFontName("font_prototype.ttf");
    price->setFontSize(40);
    price->setTextColor(cocos2d::Color4B::BLACK);
    price->setTextHorizontalAlignment(cocos2d::TextHAlignment::CENTER);
    price->setTextVerticalAlignment(cocos2d::TextVAlignment::CENTER);
    price->setString(cocos2d::StringUtils::format(" %d", _price));
    price->setPositionX(coin->getPositionX() + coin->getContentSize().width * 1.5f * 0.5f + price->getContentSize().width * 0.5f);
    price->setPositionY(getContentSize().height * 0.5f);
    
    addChild(coin);
    addChild(price);
}

void StoreItemButton::InitEquippedState()
{
    removeAllChildrenWithCleanup(true);
    loadTextures("ui/ui_shop_item-plate_btn_equipped.png", "ui/ui_shop_item-plate_btn_equipped_pressed.png");
    
    auto show = cocos2d::ProgressFromTo::create(0.3f, 0.0f, 100.0f);
    auto scale0 = cocos2d::ScaleTo::create(0.15f, 1.2f);
    auto scale1 = cocos2d::ScaleTo::create(0.15f, 1.0f);
    auto scale_ease0 = cocos2d::EaseSineOut::create(scale0);
    auto scale_ease1 = cocos2d::EaseSineIn::create(scale1);
    auto scale = cocos2d::Sequence::create(scale_ease0, scale_ease1, nullptr);
    auto effect = cocos2d::Sequence::create(show, scale, nullptr);
    
    auto mark = cocos2d::Sprite::create("ui/ui_mark_green.png");
    auto clip = cocos2d::ProgressTimer::create(mark);
    clip->setPositionX(getContentSize().width * 0.5f + 90.0f);
    clip->setPositionY(getContentSize().height * 0.5f + 10.0f);
    clip->setType(cocos2d::ProgressTimer::Type::BAR);
    clip->setBarChangeRate(cocos2d::Vec2(1.0f, 0.0f));
    clip->setPercentage(0.0f);
    clip->setMidpoint(cocos2d::Vec2(0.0f, 0.5f));
    clip->runAction(effect);
    
    addChild(clip);
}

