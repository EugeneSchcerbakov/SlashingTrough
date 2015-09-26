//
//  StoreInterface.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 04/06/15.
//
//

#include "StoreInterface.h"

#include "ScreenChanger.h"
#include "StartInterface.h"
#include "PlayerInfo.h"
#include "Utils.h"

StoreInterface* StoreInterface::create()
{
    StoreInterface *object = new StoreInterface();
    if (object && object->init()) {
        object->autorelease();
    } else {
        delete object;
        object = nullptr;
    }
    return object;
}

StoreInterface::StoreInterface()
{
}

StoreInterface::~StoreInterface()
{
}

bool StoreInterface::init()
{
    if (!cocos2d::Scene::init()) {
        return false;
    }
    
    auto director = cocos2d::Director::getInstance();
    
    cocos2d::Vec2 origin = director->getVisibleOrigin();
    cocos2d::Size frameSize = director->getVisibleSize();
    
    auto background = cocos2d::Sprite::create("ui/ui_bg.jpeg");
    background->setAnchorPoint(cocos2d::Vec2(0.0f, 0.0f));
    background->setPosition(0.0f, 0.0f);
    background->setScale(1.5f);
    
    float backBtnScale = 1.8f;
    _backBtn = cocos2d::ui::Button::create("ui/ui_btn_map.png");
    _backBtn->setScale(backBtnScale);
    _backBtn->setPositionX(origin.x + _backBtn->getContentSize().width * backBtnScale * 0.5f + 25.0f);
    _backBtn->setPositionY(origin.y + _backBtn->getContentSize().height * backBtnScale * 0.5f + 25.0f);
    _backBtn->addTouchEventListener(CC_CALLBACK_2(StoreInterface::onBackPressed, this));
    
    float settingsScale = 2.0f;
    auto settings = cocos2d::ui::Button::create("ui/ui_btn_options.png");
    settings->setScale(settingsScale);
    settings->setPositionX(origin.x + settings->getContentSize().width * settingsScale * 0.5f + 20.0f);
    settings->setPositionY(origin.y + frameSize.height - settings->getContentSize().height * settingsScale * 0.5f - 20.0f);
    
    float tabButtonsY = 0.0f;
    auto tabCallback = [&](int tab) {onCategoryChanged((Category)tab);};
    
    _tabButtons.resize(Category::AMOUNT);
    
    _tabButtons[Category::CRYSTALL] = CategoryButton::create("ui/ui_tab_crystals_inactive.png",
                                                             "ui/ui_tab_crystals_active.png", tabCallback);
    _tabButtons[Category::ARMOR] = CategoryButton::create("ui/ui_tab_armor_inactive.png",
                                                          "ui/ui_tab_armor_active.png", tabCallback);
    _tabButtons[Category::WEAPON] = CategoryButton::create("ui/ui_tab_weapons_inactive.png",
                                                           "ui/ui_tab_weapons_active.png", tabCallback);

    float width0 = _tabButtons[Category::CRYSTALL]->getContentSize().width;
    float width1 = _tabButtons[Category::ARMOR]->getContentSize().width;
    float width2 = _tabButtons[Category::WEAPON]->getContentSize().width;
    
    _tabButtons[Category::CRYSTALL]->setPositionX(frameSize.width - (width0 * 0.5f + 10.0f));
    _tabButtons[Category::CRYSTALL]->setPositionY(tabButtonsY);
    _tabButtons[Category::CRYSTALL]->setTag(Category::CRYSTALL);
    
    _tabButtons[Category::ARMOR]->setPositionX(frameSize.width - (width0 + width1 * 0.5f + 20.0f));
    _tabButtons[Category::ARMOR]->setPositionY(tabButtonsY);
    _tabButtons[Category::ARMOR]->setTag(Category::ARMOR);

    _tabButtons[Category::WEAPON]->setPositionX(frameSize.width - (width0 + width1 + width2 * 0.5f + 30.0f));
    _tabButtons[Category::WEAPON]->setPositionY(tabButtonsY);
    _tabButtons[Category::WEAPON]->setTag(Category::WEAPON);
    
    _coinsText = cocos2d::ui::Text::create("0", "font_prototype.ttf", 45);
    _coinsText->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE_LEFT);
    _coinsText->setTextColor(cocos2d::Color4B(254, 228, 146, 255));
    _coinsText->setPositionY(frameSize.height - 45);
    
    _coinsShopBtn = cocos2d::ui::Button::create("ui/ui_btn_coins_shop.png");
    _coinsShopBtn->setScale(1.8f);
    _coinsShopBtn->setPositionY(frameSize.height - _coinsShopBtn->getContentSize().height * 1.8f * 0.5f - 20.0f);
    
    _scroller = cocos2d::ui::ListView::create();
    _scroller->setItemsMargin(22.0f);
    _scroller->setGravity(cocos2d::ui::ListView::Gravity::CENTER_HORIZONTAL);
    _scroller->setClippingEnabled(true);
	_scroller->setContentSize(cocos2d::Size(frameSize.width, frameSize.height - 270.0f));
    _scroller->setPositionX(0.0f);
    _scroller->setPositionY(165.0f);
    
    scheduleUpdate();
    addChild(background, Order::BACKING);
    addChild(_scroller, Order::SCROLLER);
    addChild(_backBtn, Order::CONTROLS);
    addChild(settings, Order::CONTROLS);
    addChild(_tabButtons[Category::WEAPON], Order::CONTROLS);
    addChild(_tabButtons[Category::ARMOR], Order::CONTROLS);
    addChild(_tabButtons[Category::CRYSTALL], Order::CONTROLS);
    addChild(_coinsText, Order::CONTROLS);
    addChild(_coinsShopBtn, Order::CONTROLS);
    
    _tabButtons[Category::WEAPON]->setSelected(true);
    
    return true;
}

void StoreInterface::update(float dt)
{
    cocos2d::Size frameSize = cocos2d::Director::getInstance()->getVisibleSize();
    
    PlayerInfo &session = PlayerInfo::getInstance();
    
    int lastCoins =  atoi(_coinsText->getString().c_str());
    int nowCoins = session.getCoins();
    if (lastCoins != nowCoins) {
        _coinsText->setString(cocos2d::StringUtils::toString(nowCoins));
    }
    
    float coins_half = _coinsShopBtn->getContentSize().width * _coinsShopBtn->getScale() * 0.5f + 10.0f;
    float text_center = frameSize.width * 0.5f - _coinsText->getContentSize().width * 0.5f;
    
    _coinsText->setPositionX(text_center + coins_half);
    _coinsShopBtn->setPositionX(_coinsText->getPositionX() - coins_half);
}

void StoreInterface::onBackPressed(cocos2d::Ref *sender, cocos2d::ui::Widget::TouchEventType event)
{
    if (event == cocos2d::ui::Widget::TouchEventType::ENDED)
    {
        ScreenChanger::changeScreen(ScreenChanger::MAP);
    }
}

void StoreInterface::onCategoryChanged(Category tab)
{
    for (auto b : _tabButtons) {
        if (b->getTag() != tab) {
            b->setSelected(false);
        }
    }
    switch (tab) {
        case Category::WEAPON:
            fillScrollerWithWeapons();
            break;
        case Category::ARMOR:
            fillScrollerWithArmors();
            break;
        case Category::CRYSTALL:
            fillScrollerWithCrystalls();
            break;
        default:
            _scroller->removeAllItems();
            CC_ASSERT(false);
            break;
    }
}

void StoreInterface::fillScrollerWithWeapons()
{
    _scroller->removeAllItems();
    
    Store &store = Store::getInstance();
    Store::Items items = store.getItemsWithType(Item::Type::WEAPON);
    
    for (auto item : items) {
        StoreWeaponWidget *widget = nullptr;
        widget = StoreWeaponWidget::create(item);
        _scroller->pushBackCustomItem(widget);
    }
    
    _scroller->refreshView();
    _scroller->jumpToTop();
}

void StoreInterface::fillScrollerWithArmors()
{
    _scroller->removeAllItems();
    
    Store &store = Store::getInstance();
    Store::Items items = store.getItemsWithType(Item::Type::ARMOR);
    
    for (auto item : items) {
        StoreArmorWidget *widget = nullptr;
        widget = StoreArmorWidget::create(item);
        _scroller->pushBackCustomItem(widget);
    }
    
    _scroller->refreshView();
    _scroller->jumpToTop();
}

void StoreInterface::fillScrollerWithCrystalls()
{
    _scroller->removeAllItems();
    
    Store &store = Store::getInstance();
    Store::Items items = store.getItemsWithType(Item::Type::CRYSTALL);
    
    for (auto item : items) {
        StoreCrystallWidget *widget = nullptr;
        widget = StoreCrystallWidget::create(item);
        _scroller->pushBackCustomItem(widget);
    }
    
    _scroller->refreshView();
    _scroller->jumpToTop();
}
