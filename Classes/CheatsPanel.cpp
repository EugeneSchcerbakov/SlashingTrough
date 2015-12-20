//
//  CheatsPanel.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 20/12/15.
//
//

#include "CheatsPanel.h"

#include "Cheats.h"
#include "ui/CocosGUI.h"

const std::string CheatsPanel::panelName = "CheatsPanel";

CheatsPanel* CheatsPanel::create()
{
    CheatsPanel *cheats = new CheatsPanel();
    if (cheats && cheats->init()) {
        cheats->autorelease();
    } else {
        delete cheats;
        cheats = nullptr;
    }
    return cheats;
}

CheatsPanel::CheatsPanel()
: cocos2d::Node()
{
}

CheatsPanel::~CheatsPanel()
{
}

bool CheatsPanel::init()
{
    if (!cocos2d::Node::init()) {
        return false;
    }
    
    auto director = cocos2d::Director::getInstance();
    
    cocos2d::Size screen = director->getVisibleSize();
    _tapArea.size = cocos2d::Size(200.0, 100.0f);
    _tapArea.origin.x = screen.width - _tapArea.size.width;
    _tapArea.origin.y = screen.height - _tapArea.size.height;
    
    auto input = cocos2d::EventListenerTouchOneByOne::create();
    input->onTouchBegan = CC_CALLBACK_2(CheatsPanel::touchBegan, this);
    input->onTouchEnded = CC_CALLBACK_2(CheatsPanel::touchEnded, this);
    input->onTouchCancelled = CC_CALLBACK_2(CheatsPanel::touchCanceled, this);
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(input, this);
    
    return true;
}

void CheatsPanel::createPanel()
{
    auto director = cocos2d::Director::getInstance();
    auto dispatcher = director->getEventDispatcher();
    
    cocos2d::Vec2 origin = director->getVisibleOrigin();
    cocos2d::Size screen = director->getVisibleSize();
    _panelRect.size = cocos2d::Size(550.0f, 200.0f);
    _panelRect.origin.x = origin.x + screen.width * 0.5f - _panelRect.size.width * 0.5f;
    _panelRect.origin.y = origin.y + screen.height * 0.5f - _panelRect.size.height * 0.5f + 350.0f;
    
    auto layer = cocos2d::Layer::create();
    layer->setName(panelName);
    
    // block touches propagation
    auto inputLocker = cocos2d::EventListenerTouchOneByOne::create();
    inputLocker->setSwallowTouches(true);
    
    inputLocker->onTouchBegan = [](cocos2d::Touch* touch, cocos2d::Event* event)
    {
        return true;
    };
    
    inputLocker->onTouchEnded = [this](cocos2d::Touch* touch, cocos2d::Event* event)
    {
        cocos2d::Vec2 tap = touch->getLocation();
        if (panelOnScreen() && !_panelRect.containsPoint(tap))
        {
            destroyPanel();
        }
    };
    
    dispatcher->addEventListenerWithSceneGraphPriority(inputLocker, layer);
    
    auto tint = cocos2d::Sprite::create("ui/ui_bg_tint.png");
    tint->setScale(4.0f);
    
    auto list = cocos2d::ui::ListView::create();
    list->setItemsMargin(37.0f);
    list->setGravity(cocos2d::ui::ListView::Gravity::CENTER_HORIZONTAL);
    list->setScrollBarEnabled(true);
    list->setClippingEnabled(true);
    list->setContentSize(_panelRect.size);
    list->setBackGroundColorType(cocos2d::ui::Layout::BackGroundColorType::SOLID);
    list->setBackGroundColor(cocos2d::Color3B::GRAY);
    list->setPositionX(_panelRect.origin.x);
    list->setPositionY(_panelRect.origin.y);
    
    Cheats &cheats = Cheats::getInstance();
    auto allCheats = cheats.getAll();
    
    for(const auto& pair : allCheats)
    {
        const Cheats::Info& desc = pair.second;
        
        auto func = [](cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType event)
        {
            if (event == cocos2d::ui::Widget::TouchEventType::ENDED)
            {
                auto sender = dynamic_cast<cocos2d::ui::Widget *>(ref);
                if (sender) {
                    std::string id = sender->getName();
                    Cheats::getInstance().runCheat(id);
                }
            }
        };
        
        auto button = cocos2d::ui::Button::create();
        button->setTitleText(desc.title);
        button->setTitleColor(cocos2d::Color3B::BLACK);
        button->setTitleFontSize(30.0f);
        button->addTouchEventListener(func);
        button->setName(desc.id);
        
        list->pushBackCustomItem(button);
    }
    
    layer->addChild(tint, 0);
    layer->addChild(list, 1);
    
    addChild(layer);
}

void CheatsPanel::destroyPanel()
{
    if (!panelOnScreen()) {
        return;
    }
    
    removeChildByName(panelName);
}

bool CheatsPanel::panelOnScreen() const
{
    return getChildByName(panelName) != nullptr;
}

bool CheatsPanel::touchBegan(cocos2d::Touch *touch, cocos2d::Event *e)
{
    _touchDown = touch->getLocation();
    
    return true;
}

void CheatsPanel::touchEnded(cocos2d::Touch *touch, cocos2d::Event *e)
{
    if (panelOnScreen()) {
        return;
    }
    
    cocos2d::Vec2 tap = touch->getLocation();
    
    if (_tapArea.containsPoint(_touchDown) && _tapArea.containsPoint(tap))
    {
        createPanel();
    }
}

void CheatsPanel::touchCanceled(cocos2d::Touch *touch, cocos2d::Event *e)
{
    
}
