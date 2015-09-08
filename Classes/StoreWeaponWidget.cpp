//
//  StoreWeaponWidget.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 06/06/15.
//
//

#include "StoreWeaponWidget.h"

StoreWeaponWidget* StoreWeaponWidget::create(Item::WeakPtr item)
{
    StoreWeaponWidget *widget = new StoreWeaponWidget(item);
    if (widget && widget->init()) {
        widget->autorelease();
    } else {
        delete widget;
        widget = nullptr;
    }
    return widget;
}

StoreWeaponWidget::StoreWeaponWidget(Item::WeakPtr item)
: StoreItemBaseWidget(item)
{
}

StoreWeaponWidget::~StoreWeaponWidget()
{
}

bool StoreWeaponWidget::init()
{
    if (!StoreItemBaseWidget::init()) {
        return false;
    }
    
    ItemWeapon *weapon = ItemWeapon::cast(_item.lock());
    
    int price = weapon->getPrice();
    if (price > 0)
    {
        auto priceIcon = cocos2d::Sprite::create("icons/icon_coin.png");
        priceIcon->setScale(1.8f);
    
        std::string priceString = cocos2d::StringUtils::format("%d", price);
        auto priceText = cocos2d::ui::Text::create(priceString, "font_prototype.ttf", 50);
        priceText->setTextColor(cocos2d::Color4B(76, 17, 49, 255));
        priceText->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE_LEFT);
        priceText->setPositionX(getContentSize().width * 0.5f - 60.0f);
        priceText->setPositionY(priceText->getContentSize().height * 0.5f + 20.0f);
        priceText->addChild(priceIcon);
        priceIcon->setPositionX(-priceIcon->getContentSize().width * 1.8f * 0.5f);
        priceIcon->setPositionY(priceIcon->getContentSize().height * 1.8f * 0.5f);
    
        addChild(priceText, 0);
    }
    
    std::string damageStr = cocos2d::StringUtils::format("%d", (int)weapon->getDamage());
    auto damageText = cocos2d::ui::Text::create(damageStr, "font_prototype.ttf", 40);
    damageText->setTextColor(cocos2d::Color4B(76, 17, 49, 255));
    damageText->setPositionX(damageText->getContentSize().width * 0.5f + 13.0f);
    damageText->setPositionY(getContentSize().height - damageText->getContentSize().height * 0.5f - 10.0f);
    
    addChild(damageText, 1);
    
    return true;
}

std::string StoreWeaponWidget::chooseSpeedLabel(float speed)
{
    const float fast = 0.2f;
    const float medi = 0.45f;
    
    std::string label;
    if (speed <= fast) {
        label = "Fast";
    } else if (speed <= medi) {
        label = "Medium";
    } else {
        label = "Slow";
    }
    
    return label;
}
