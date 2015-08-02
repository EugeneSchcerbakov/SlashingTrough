//
//  StoreInterface.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 04/06/15.
//
//

#include "StoreInterface.h"

#include "SessionInfo.h"
#include "Utils.h"

StoreInterface* StoreInterface::create(const std::string &prevSceneName)
{
    StoreInterface *object = new StoreInterface(prevSceneName);
    if (object && object->init()) {
        object->autorelease();
    } else {
        delete object;
        object = nullptr;
    }
    return object;
}

StoreInterface::StoreInterface(const std::string &prevSceneName)
: _prevSceneName(prevSceneName)
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
    
    cocos2d::Color4B backingColor(255, 229, 153, 255);
    cocos2d::LayerColor *backging = cocos2d::LayerColor::create(backingColor);
    
    cocos2d::Size frameSize = cocos2d::Director::getInstance()->getVisibleSize();
    
    cocos2d::Sprite *decorTop = cocos2d::Sprite::create("ui/ui_shop_wood-frame_top.png");
    decorTop->setPositionX(frameSize.width * 0.5f);
    decorTop->setPositionY(frameSize.height - decorTop->getContentSize().height * 0.5f);
    
    cocos2d::Sprite *decorBtm = cocos2d::Sprite::create("ui/ui_shop_wood-frame_down.png");
    decorBtm->setPositionX(frameSize.width * 0.5f);
    decorBtm->setPositionY(decorBtm->getContentSize().height * 0.5f);
    
    float backBtnScale = 1.8f;
    _backBtn = cocos2d::ui::Button::create("ui/ui_btn_back.png");
    _backBtn->setScale(backBtnScale);
    _backBtn->setPositionX(_backBtn->getContentSize().width * backBtnScale * 0.5f);
    _backBtn->setPositionY(_backBtn->getContentSize().height * backBtnScale * 0.5f);
    _backBtn->addTouchEventListener(CC_CALLBACK_2(StoreInterface::onBackPressed, this));
    
    float tabButtonsY = 190.0f;
    auto tabCallback = [&](int tab) {onCategoryChanged((Category)tab);};
    
    _tabButtons.resize(Category::AMOUNT);
    _tabButtons[Category::WEAPON] = CategoryButton::create("ui/ui_shop_weapon_tab.png", tabCallback);
    _tabButtons[Category::WEAPON]->setPositionX(150.0f);
    _tabButtons[Category::WEAPON]->setPositionY(tabButtonsY);
    _tabButtons[Category::WEAPON]->setTag(Category::WEAPON);
    
    _tabButtons[Category::ARMOR] = CategoryButton::create("ui/ui_shop_armor_tab.png", tabCallback);
    _tabButtons[Category::ARMOR]->setPositionX(320.0f);
    _tabButtons[Category::ARMOR]->setPositionY(tabButtonsY - 23.0f);
    _tabButtons[Category::ARMOR]->setTag(Category::ARMOR);
    
    _tabButtons[Category::BOOSTERS] = CategoryButton::create("ui/ui_shop_boost_tab.png", tabCallback);
    _tabButtons[Category::BOOSTERS]->setPositionX(490.0f);
    _tabButtons[Category::BOOSTERS]->setPositionY(tabButtonsY);
    _tabButtons[Category::BOOSTERS]->setTag(Category::BOOSTERS);
    
    _coinsShopBtn = cocos2d::ui::Button::create("ui/ui_btn_coins_shop.png");
    _coinsShopBtn->setScale(1.8f);
    _coinsShopBtn->setPositionX(frameSize.width * 0.5f - 200.0f);
    _coinsShopBtn->setPositionY(frameSize.height - _coinsShopBtn->getContentSize().height * 1.8f * 0.5f - 20.0f);
    
    cocos2d::Sprite *coinIcon = cocos2d::Sprite::create("icons/icon_coin.png");
    coinIcon->setScale(1.8f);
    coinIcon->setPositionX(frameSize.width * 0.5f - 120.0f);
    coinIcon->setPositionY(frameSize.height - coinIcon->getContentSize().height * 1.8 * 0.5f - 20.0f);
    
    cocos2d::Sprite *damageIcon = cocos2d::Sprite::create("icons/icon_dmg.png");
    damageIcon->setScale(1.8f);
    damageIcon->setPositionX(frameSize.width * 0.5f + 170.0f);
    damageIcon->setPositionY(frameSize.height - damageIcon->getContentSize().height * 1.8 * 0.5f - 20.0f);
    
    _coinsText = cocos2d::ui::Text::create();
    _coinsText->setFontName("font_prototype.ttf");
    _coinsText->setFontSize(45);
    _coinsText->setTextColor(cocos2d::Color4B(254, 228, 146, 255));
    _coinsText->setTextHorizontalAlignment(cocos2d::TextHAlignment::CENTER);
    _coinsText->setTextVerticalAlignment(cocos2d::TextVAlignment::CENTER);
    _coinsText->setString("0");
    _coinsText->setAnchorPoint(cocos2d::Vec2(0.0f, 0.5f));
    _coinsText->setPositionX(coinIcon->getPositionX() +
                             coinIcon->getContentSize().width * coinIcon->getScale() * 0.5f + 10.0f);
    _coinsText->setPositionY(frameSize.height - 45);
    
    _damageText = cocos2d::ui::Text::create();
    _damageText->setFontName("font_prototype.ttf");
    _damageText->setFontSize(45);
    _damageText->setTextColor(cocos2d::Color4B(254, 228, 146, 255));
    _damageText->setTextHorizontalAlignment(cocos2d::TextHAlignment::CENTER);
    _damageText->setTextVerticalAlignment(cocos2d::TextVAlignment::CENTER);
    _damageText->setString("0");
    _damageText->setAnchorPoint(cocos2d::Vec2(0.0f, 0.5f));
    _damageText->setPositionX(damageIcon->getPositionX() +
                              damageIcon->getContentSize().width * damageIcon->getScale() * 0.5f + 10.0f);
    _damageText->setPositionY(frameSize.height - 45);
    
    _scroller = cocos2d::ui::ListView::create();
    _scroller->setItemsMargin(15.0f);
    _scroller->setGravity(cocos2d::ui::ListView::Gravity::CENTER_HORIZONTAL);
    _scroller->setClippingEnabled(false);
	_scroller->setContentSize(cocos2d::Size(frameSize.width, frameSize.height - 400.0f));
    _scroller->setPositionX(0.0f);
    _scroller->setPositionY(270.0f);
    
    scheduleUpdate();
    addChild(backging, Order::BACKING);
    addChild(decorTop, Order::DECORATION);
    addChild(decorBtm, Order::DECORATION);
    addChild(_scroller, Order::SCROLLER);
    addChild(_backBtn, Order::CONTROLS);
    addChild(_tabButtons[Category::WEAPON], Order::CONTROLS);
    addChild(_tabButtons[Category::ARMOR], Order::CONTROLS);
    addChild(_tabButtons[Category::BOOSTERS], Order::CONTROLS);
    addChild(_coinsText, Order::CONTROLS);
    addChild(_damageText, Order::CONTROLS);
    addChild(coinIcon, Order::CONTROLS);
    addChild(damageIcon, Order::CONTROLS);
    addChild(_coinsShopBtn, Order::CONTROLS);
    
    _tabButtons[Category::WEAPON]->setSelected(true);
    
    return true;
}

void StoreInterface::update(float dt)
{
    SessionInfo &session = SessionInfo::getInstance();
    
    int lastCoins =  atoi(_coinsText->getString().c_str());
    int nowCoins = session.getCoins();
    if (lastCoins != nowCoins) {
        _coinsText->setString(cocos2d::StringUtils::toString(nowCoins));
    }
    
    Equip::Ptr ptr = Store::getInstance().getItemById(session.getEquippedWeaponId());
    if (ptr) {
        EquipWeapon *wpn = EquipWeapon::cast(ptr);
        int lastDamage = atoi(_damageText->getString().c_str());
        int nowDamage = wpn->damage;
        if (lastDamage != nowDamage) {
            _damageText->setString(cocos2d::StringUtils::toString(nowDamage));
        }
    } else {
        CC_ASSERT(false);
    }
}

void StoreInterface::onBackPressed(cocos2d::Ref *sender, cocos2d::ui::Widget::TouchEventType event)
{
    if (event == cocos2d::ui::Widget::TouchEventType::ENDED)
    {
        SessionInfo &profile = SessionInfo::getInstance();
        Equip::Ptr ptr = Store::getInstance().getItemById(profile.getEquippedWeaponId());
        EquipWeapon *wpn = EquipWeapon::cast(ptr);
        misc::luaSetGlobalInteger("PlayerTotalGoldPoints", profile.getCoins());
        misc::luaSetGlobalInteger("PlayerTotalDamagePoints", (int)wpn->damage);
        misc::luaSetGlobalInteger("PlayerBestResultGoldPoints", profile.getBestScore().coins);
        misc::luaSetGlobalInteger("PlayerBestResultKillPoints", profile.getBestScore().kills);
        
        cocos2d::Director *director = cocos2d::Director::getInstance();
        std::string startSceneName = misc::luaGetGlobalString("StartScreenSceneName");
        std::string resultSceneName = misc::luaGetGlobalString("ResultScreenSceneName");
        if (_prevSceneName == startSceneName) {
            auto scene = misc::makeSceneFromLua("CreateStartscreenScene");
            auto trans = cocos2d::TransitionSlideInL::create(0.3f, scene);
            director->replaceScene(trans);
        } else if (_prevSceneName == resultSceneName) {
            auto scene = misc::makeSceneFromLua("CreateResultScene");
            auto trans = cocos2d::TransitionSlideInL::create(0.3f, scene);
            director->replaceScene(trans);
        } else {
            CC_ASSERT(false);
        }
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
        case Category::BOOSTERS:
            fillScrollerWithBoosters();
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
    Store::Items items = store.getWeaponItems();
    
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
    Store::Items items = store.getArmorItems();
    
    for (auto item : items) {
        StoreArmorWidget *widget = nullptr;
        widget = StoreArmorWidget::create(item);
        _scroller->pushBackCustomItem(widget);
    }
    
    _scroller->refreshView();
    _scroller->jumpToTop();
}

void StoreInterface::fillScrollerWithBoosters()
{
    _scroller->removeAllItems();
}


