//
//  StoreWeaponWidget.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 06/06/15.
//
//

#include "StoreWeaponWidget.h"

#include "SessionInfo.h"

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
: _item(item)
{
}

StoreWeaponWidget::~StoreWeaponWidget()
{
}

bool StoreWeaponWidget::init()
{
    if (!cocos2d::ui::Layout::init()) {
        return false;
    }
    
    EquipWeapon *weapon = EquipWeapon::cast(_item.lock());
    
    setBackGroundImageScale9Enabled(false);
    setBackGroundImage("ui/ui_shop_item-plate.png");
    setContentSize(getBackGroundImageTextureSize());
    setAnchorPoint(cocos2d::Vec2(0.5f, 0.5f));
    
    auto *itemIcon = cocos2d::Sprite::create(weapon->icon);
    itemIcon->setScale(1.4f);
    
    auto itemName = cocos2d::ui::Text::create();
    itemName->setFontName("font_prototype.ttf");
    itemName->setFontSize(27);
    itemName->setTextColor(cocos2d::Color4B::BLACK);
    itemName->setTextHorizontalAlignment(cocos2d::TextHAlignment::CENTER);
    itemName->setTextVerticalAlignment(cocos2d::TextVAlignment::CENTER);
    itemName->setString(weapon->name);
    
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
    damageText->setString(cocos2d::StringUtils::format(" %d", (int)floorf(weapon->damage)));
    damageText->setPositionX(damageIcon->getPositionX() + damageIcon->getContentSize().width);
    damageText->setPositionY(damageText->getContentSize().height * 0.5f);
    
    damage->addChild(damageIcon);
    damage->addChild(damageText);
    
    std::string speedLabel = chooseSpeedLabel(weapon->speed);
    
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
    
    auto desc = cocos2d::ui::Text::create();
    desc->setFontName("font_prototype.ttf");
    desc->setFontSize(25);
    desc->setTextColor(cocos2d::Color4B(150, 87, 38, 255));
    desc->setTextHorizontalAlignment(cocos2d::TextHAlignment::LEFT);
    desc->setTextVerticalAlignment(cocos2d::TextVAlignment::TOP);
    desc->setString(weapon->desc);
    desc->setPositionX(getContentSize().width * 0.5f - 45.0f);
    desc->setPositionY(115.0f);
    desc->setAnchorPoint(cocos2d::Vec2(0.0f, 0.5f));
    
    _button = StoreItemButton::create();
    _button->setPositionX(getContentSize().width - _button->getContentSize().width * 0.5f);
    _button->setPositionY(_button->getContentSize().height * 0.5f);
    _button->addTouchEventListener(CC_CALLBACK_2(StoreWeaponWidget::onBuyPressed, this));
    _button->setPrice(weapon->price);
    _button->switchState(StoreItemButton::State::BUY);

    scheduleUpdate();
    addChild(_button, 0);
    addChild(iconPanel, 1);
    addChild(damage, 2);
    addChild(speed, 2);
    addChild(desc, 2);
    
    return true;
}

void StoreWeaponWidget::update(float dt)
{
    Equip::Ptr ptr = _item.lock();
    SessionInfo &save = SessionInfo::getInstance();
    
    if (save.isEquipOwned(ptr->id) && !save.isWeaponEquipped(ptr->id) && !_button->isState(StoreItemButton::State::EQUIP))
    {
        _button->switchState(StoreItemButton::State::EQUIP);
    }
    if (save.isWeaponEquipped(ptr->id) && !_button->isState(StoreItemButton::State::EQUIPPED))
    {
        _button->switchState(StoreItemButton::State::EQUIPPED);
    }
}

void StoreWeaponWidget::onBuyPressed(cocos2d::Ref *sender, cocos2d::ui::Widget::TouchEventType event)
{
    if (event == cocos2d::ui::Widget::TouchEventType::ENDED)
    {
        Equip::Ptr ptr = _item.lock();
        SessionInfo &profile = SessionInfo::getInstance();
        
        bool needSave = false;
        if (!profile.isEquipOwned(ptr->id)) {
            Store::getInstance().buy(ptr->id);
            needSave = true;
        } else if (!profile.isWeaponEquipped(ptr->id)) {
            profile.equipWeapon(ptr);
            needSave = true;
        }
        
        if (needSave) {
            profile.save();
        }
    }
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

