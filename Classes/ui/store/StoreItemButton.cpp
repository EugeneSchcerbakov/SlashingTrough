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
        initBuyState();
    } else if (state == State::EQUIP) {
        _state = state;
        removeAllChildrenWithCleanup(true);
        loadTextureNormal("ui/ui_btn_equip_status.png");
    } else if (state == State::EQUIPPED) {
        _state = state;
        initEquippedState();
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

void StoreItemButton::initBuyState()
{
    removeAllChildrenWithCleanup(true);
    loadTextureNormal("ui/ui_btn_buy_status.png");
}

void StoreItemButton::initEquippedState()
{
    removeAllChildrenWithCleanup(true);
    loadTextureNormal("ui/ui_btn_equipped_status.png");
}
