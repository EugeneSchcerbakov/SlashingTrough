//
//  MapInterface.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 23/08/15.
//
//

#include "MapInterface.h"

#include "ScreenChanger.h"
#include "LevelsCache.h"
#include "PlayerInfo.h"

MapInterface* MapInterface::create()
{
    MapInterface *map = new MapInterface();
    if (map && map->init()) {
        map->autorelease();
    } else {
        delete map;
        map = nullptr;
    }
    return map;
}

MapInterface::MapInterface()
{
}

MapInterface::~MapInterface()
{
}

bool MapInterface::init()
{
    if (!PopupCarrier::init()) {
        return false;
    }
    
    PlayerInfo &player = PlayerInfo::getInstance();
    
    cocos2d::Vec2 origin = cocos2d::Director::getInstance()->getVisibleOrigin();
    cocos2d::Size size = cocos2d::Director::getInstance()->getVisibleSize();
    
    _background = cocos2d::LayerColor::create(cocos2d::Color4B::BLACK);
    _guiLayer = cocos2d::Layer::create();
    _mapWidget = MapWidget::create("map.xml");
    _mapWidget->setSwallowTouches(false);
    
    auto input = cocos2d::EventListenerTouchOneByOne::create();
    input->onTouchBegan = CC_CALLBACK_2(MapInterface::mapTouchBegan, this);
    input->onTouchEnded = CC_CALLBACK_2(MapInterface::mapTouchEnded, this);
    input->onTouchCancelled = CC_CALLBACK_2(MapInterface::mapTouchCanceled, this);
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(input, _guiLayer);
    
    cocos2d::Sprite *coinIcon = cocos2d::Sprite::create("icons/icon_coin.png");
    coinIcon->setScale(1.8f);
    float coinsIconWidth = coinIcon->getContentSize().width * coinIcon->getScale();
    
    std::string coinsString = cocos2d::StringUtils::format("%d", player.getCoins());
    _coinsText = cocos2d::ui::Text::create("", "font_prototype.ttf", 45);
    _coinsText->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE_LEFT);
    _coinsText->setTextColor(cocos2d::Color4B(254, 228, 146, 255));
    _coinsText->setTextHorizontalAlignment(cocos2d::TextHAlignment::CENTER);
    _coinsText->setTextVerticalAlignment(cocos2d::TextVAlignment::CENTER);
    _coinsText->setString(coinsString);
    _coinsText->setAnchorPoint(cocos2d::Vec2(0.0f, 0.5f));
    _coinsText->setPositionX(size.width * 0.5f - _coinsText->getContentSize().width * 0.5f + coinsIconWidth * 0.5f);
    _coinsText->setPositionY(size.height - 45);
    
    coinIcon->setPositionX(_coinsText->getPositionX() - coinsIconWidth * 0.5f);
    coinIcon->setPositionY(size.height - coinIcon->getContentSize().height * 1.8f * 0.5f - 20.0f);
    
    auto onShopPressed = [](cocos2d::Ref *ref, cocos2d::ui::Widget::TouchEventType e)
    {
        if (e == cocos2d::ui::Widget::TouchEventType::ENDED) {
            ScreenChanger::changeScreen(ScreenChanger::STORE);
        }
    };
    
    auto onDailyPressed = [&](cocos2d::Ref *ref, cocos2d::ui::Widget::TouchEventType e)
    {
        if (e == cocos2d::ui::Widget::TouchEventType::ENDED) {
            auto popup = DailyMissionPopup::create();
            pushPopup(popup, Order::POPUP, "DailyMissionPopup");
        }
    };
    
    float btnScale = 1.8f;
    auto shopButton = cocos2d::ui::Button::create("ui/ui_btn_shop.png");
    shopButton->setScale(btnScale);
    shopButton->setPositionX(origin.x + shopButton->getContentSize().width * btnScale * 0.5f + 25.0f);
    shopButton->setPositionY(origin.y + shopButton->getContentSize().height * btnScale * 0.5f + 25.0f);
    shopButton->addTouchEventListener(onShopPressed);
    
    auto dailyButton = cocos2d::ui::Button::create("ui/ui_btn_daily-missions.png");
    dailyButton->setScale(btnScale);
    dailyButton->setPositionX(shopButton->getPositionX() + 100.0f);
    dailyButton->setPositionY(origin.y + dailyButton->getContentSize().height * btnScale * 0.5f + 25.0f);
    dailyButton->addTouchEventListener(onDailyPressed);

    float settingsScale = 2.0f;
    auto settings = cocos2d::ui::Button::create("ui/ui_btn_options.png");
    settings->setScale(settingsScale);
    settings->setPositionX(origin.x + settings->getContentSize().width * settingsScale * 0.5f + 20.0f);
    settings->setPositionY(origin.y + size.height - settings->getContentSize().height * settingsScale * 0.5f - 20.0f);
    
    _guiLayer->addChild(coinIcon);
    _guiLayer->addChild(_coinsText);
    _guiLayer->addChild(shopButton);
    _guiLayer->addChild(dailyButton);
    _guiLayer->addChild(settings);
    
    addChild(_background, Order::COLOR);
    addChild(_mapWidget, Order::MAP);
    addChild(_guiLayer, Order::CONTROLS);
    
    attachHandlerWithZOrder(Order::CONTROLS);
    
    return true;
}

bool MapInterface::mapTouchBegan(cocos2d::Touch *touch, cocos2d::Event *e)
{
    return true;
}

void MapInterface::mapTouchEnded(cocos2d::Touch *touch, cocos2d::Event *e)
{
    cocos2d::Vec2 location = touch->getLocation();
    MapLevelMark *mark = _mapWidget->getLevelUnderPoint(location);
    if (mark)
    {
        auto level = mark->getLevel().lock();
        if (!level->isStatus(FieldLevel::Status::LOCKED)) {
            std::string levelId = level->getId();
            std::string title = "Mission " + mark->getLevelText();
            
            LevelsCache &levelsCache = LevelsCache::getInstance();
            
            FieldLevel::WeakPtr level = levelsCache.getLevelById(levelId);
            MissionStartPopup *popup = MissionStartPopup::create(levelId, title);
            
            pushPopup(popup, Order::POPUP, "MissionStartPopup");
        } else {
            // shop pop up text
        }
    }
}

void MapInterface::mapTouchCanceled(cocos2d::Touch *touch, cocos2d::Event *e)
{
}
