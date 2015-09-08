//
//  StoreArmorWidget.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 02/08/15.
//
//

#include "StoreArmorWidget.h"

StoreArmorWidget* StoreArmorWidget::create(Item::WeakPtr item)
{
    StoreArmorWidget *widget = new StoreArmorWidget(item);
    if (widget && widget->init()) {
        widget->autorelease();
    } else {
        delete widget;
        widget = nullptr;
    }
    return widget;
}

StoreArmorWidget::StoreArmorWidget(Item::WeakPtr item)
: StoreItemBaseWidget(item)
{
}

StoreArmorWidget::~StoreArmorWidget()
{
}

bool StoreArmorWidget::init()
{
    if (!StoreItemBaseWidget::init()) {
        return false;
    }
    
    ItemArmor *armor = ItemArmor::cast(_item.lock());
    
    int price = armor->getPrice();
    if (price > 0)
    {
        auto priceIcon = cocos2d::Sprite::create("icons/icon_coin.png");
        priceIcon->setScale(1.8f);
        
        std::string priceString = cocos2d::StringUtils::format("%d", price);
        auto priceText = cocos2d::ui::Text::create(priceString, "font_prototype.ttf", 53);
        priceText->setTextColor(cocos2d::Color4B(76, 17, 49, 255));
        priceText->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE_LEFT);
        priceText->setPositionX(getContentSize().width * 0.5f - 60.0f);
        priceText->setPositionY(priceText->getContentSize().height * 0.5f + 20.0f);
        priceText->addChild(priceIcon);
        priceIcon->setPositionX(-priceIcon->getContentSize().width * 1.8f * 0.5f);
        priceIcon->setPositionY(priceIcon->getContentSize().height * 1.8f * 0.5f);
        
        addChild(priceText);
    }
    
    std::string protectStr = cocos2d::StringUtils::format("%d", (int)armor->getExtraHealth());
    auto protectText = cocos2d::ui::Text::create(protectStr, "font_prototype.ttf", 40);
    protectText->setTextColor(cocos2d::Color4B(76, 17, 49, 255));
    protectText->setPositionX(protectText->getContentSize().width * 0.5f + 13.0f);
    protectText->setPositionY(getContentSize().height - protectText->getContentSize().height * 0.5f - 10.0f);
    
    addChild(protectText, 1);
    
    return true;
}
