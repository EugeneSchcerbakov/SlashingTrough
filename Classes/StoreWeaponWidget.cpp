//
//  StoreWeaponWidget.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 06/06/15.
//
//

#include "StoreWeaponWidget.h"

StoreWeaponWidget* StoreWeaponWidget::create(Equip::WeakPtr item)
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

StoreWeaponWidget::StoreWeaponWidget(Equip::WeakPtr item)
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
    
    EquipWeapon *weapon = EquipWeapon::cast(_item.lock());
    
    auto damage = cocos2d::ui::Text::create();
    damage->setFontName("font_prototype.ttf");
    damage->setFontSize(33);
    damage->setTextColor(cocos2d::Color4B::BLACK);
    damage->setTextHorizontalAlignment(cocos2d::TextHAlignment::CENTER);
    damage->setTextVerticalAlignment(cocos2d::TextVAlignment::CENTER);
    damage->setString("Damage............");
    damage->setPositionX(getContentSize().width * 0.5f - 45.0f);
    damage->setPositionY(220.0f);
    damage->setAnchorPoint(cocos2d::Vec2(0.0f, 0.5f));
    
    auto damageIcon = cocos2d::Sprite::create("icons/icon_dmg.png");
    damageIcon->setScale(1.5f);
    damageIcon->setPositionX(damage->getContentSize().width + damageIcon->getContentSize().width * 1.5f * 0.5f);
    damageIcon->setPositionY(damageIcon->getContentSize().height * 1.5f * 0.5f);
    
    auto damageText = cocos2d::ui::Text::create();
    damageText->setFontName("font_prototype.ttf");
    damageText->setFontSize(45);
    damageText->setTextColor(cocos2d::Color4B::BLACK);
    damageText->setTextHorizontalAlignment(cocos2d::TextHAlignment::LEFT);
    damageText->setTextVerticalAlignment(cocos2d::TextVAlignment::CENTER);
    damageText->setString(cocos2d::StringUtils::format("   %d", (int)floorf(weapon->getDamage())));
    damageText->setPositionX(damageIcon->getPositionX() + damageIcon->getContentSize().width);
    damageText->setPositionY(damageText->getContentSize().height * 0.5f);
    
    damage->addChild(damageIcon);
    damage->addChild(damageText);
    
    std::string speedLabel = chooseSpeedLabel(weapon->getSpeed());
    
    auto speed = cocos2d::ui::Text::create();
    speed->setFontName("font_prototype.ttf");
    speed->setFontSize(33);
    speed->setTextColor(cocos2d::Color4B::BLACK);
    speed->setTextHorizontalAlignment(cocos2d::TextHAlignment::LEFT);
    speed->setTextVerticalAlignment(cocos2d::TextVAlignment::CENTER);
    speed->setString(cocos2d::StringUtils::format("Speed................%s", speedLabel.c_str()));
    speed->setPositionX(getContentSize().width * 0.5f - 45.0f);
    speed->setPositionY(170.0f);
    speed->setAnchorPoint(cocos2d::Vec2(0.0f, 0.5f));
    
    addChild(damage, 2);
    addChild(speed, 2);
    
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

