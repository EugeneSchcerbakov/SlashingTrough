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
    
    auto defence = cocos2d::ui::Text::create();
    defence->setFontName("font_prototype.ttf");
    defence->setFontSize(33);
    defence->setTextColor(cocos2d::Color4B::BLACK);
    defence->setTextHorizontalAlignment(cocos2d::TextHAlignment::CENTER);
    defence->setTextVerticalAlignment(cocos2d::TextVAlignment::CENTER);
    defence->setString("Defence...........");
    defence->setPositionX(getContentSize().width * 0.5f - 45.0f);
    defence->setPositionY(220.0f);
    defence->setAnchorPoint(cocos2d::Vec2(0.0f, 0.5f));
    
    auto defenceIcon = cocos2d::Sprite::create("icons/icon_health.png");
    defenceIcon->setScale(1.0f);
    defenceIcon->setPositionX(defence->getContentSize().width + defenceIcon->getContentSize().width * 1.5f * 0.5f);
    defenceIcon->setPositionY(defenceIcon->getContentSize().height * 0.5f);
    
    auto defenceText = cocos2d::ui::Text::create();
    defenceText->setFontName("font_prototype.ttf");
    defenceText->setFontSize(45);
    defenceText->setTextColor(cocos2d::Color4B::BLACK);
    defenceText->setTextHorizontalAlignment(cocos2d::TextHAlignment::LEFT);
    defenceText->setTextVerticalAlignment(cocos2d::TextVAlignment::CENTER);
    defenceText->setString(cocos2d::StringUtils::format("   +%d", (int)floorf(armor->getExtraHealth())));
    defenceText->setPositionX(defenceIcon->getPositionX() + defenceIcon->getContentSize().width);
    defenceText->setPositionY(defenceText->getContentSize().height * 0.5f);
    
    defence->addChild(defenceIcon);
    defence->addChild(defenceText);
    
    addChild(defence, 2);
    
    return true;
}
