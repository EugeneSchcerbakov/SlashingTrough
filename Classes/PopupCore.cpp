//
//  PopupCore.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 05/10/15.
//
//

#include "PopupCore.h"

#include "Log.h"

Popup::Popup()
: cocos2d::ui::Layout()
, _closing(false)
{
}

Popup::~Popup()
{
    auto dispatcher = getEventDispatcher();
    dispatcher->removeEventListenersForTarget(this);
}

void Popup::showEffect()
{
    if (_tint->getNumberOfRunningActions() > 0) {
        return;
    }
    
    auto fadein = cocos2d::FadeIn::create(0.2f);
    _tint->setOpacity(0);
    _tint->runAction(fadein);
}

float Popup::hideEffect()
{
    if (_tint->getNumberOfRunningActions() > 0) {
        return 0.0f;
    }
    
    float time = 0.1f;
    
    auto fadeout = cocos2d::FadeOut::create(time);
    _tint->runAction(fadeout);
    
    return time;
}

void Popup::setClose(bool close)
{
    _closing = close;
}

bool Popup::isClosing() const
{
    return _closing;
}

bool Popup::init()
{
    if (!cocos2d::ui::Layout::init()) {
        return false;
    }
    
    auto dispatcher = getEventDispatcher();
    
    // block touches propagation
    _inputLocker = cocos2d::EventListenerTouchOneByOne::create();
    _inputLocker->setSwallowTouches(true);
    _inputLocker->onTouchBegan = [](cocos2d::Touch* touch, cocos2d::Event* event){return true;};
    
    dispatcher->addEventListenerWithSceneGraphPriority(_inputLocker, this);
    
    // black underlayer
    _tint = cocos2d::Sprite::create("ui/ui_bg_tint.png");
    _tint->setScale(4.0f);
    
    addChild(_tint, 0);
    
    return true;
}

PopupCarrier::PopupCarrier()
: cocos2d::Scene()
{
}

PopupCarrier::~PopupCarrier()
{
}

PopupCarrier* PopupCarrier::create()
{
    PopupCarrier *carrier = new PopupCarrier();
    if (carrier && carrier->init()) {
        carrier->autorelease();
    } else {
        delete carrier;
        carrier = nullptr;
    }
    return carrier;
}

void PopupCarrier::attachHandlerWithZOrder(int zOrder)
{
    cocos2d::Layer *layer = cocos2d::Layer::create();
    
    addChild(layer, zOrder);
    
    auto handler = cocos2d::EventListenerTouchOneByOne::create();
    handler->onTouchBegan = CC_CALLBACK_2(PopupCarrier::touchBegan, this);
    
    auto dispatcher = getEventDispatcher();
    dispatcher->addEventListenerWithSceneGraphPriority(handler, layer);
}

void PopupCarrier::pushPopup(Popup *popup, int zOrder, std::string name)
{
    if (!popup) {
        WRITE_WARN("Failed to add invalid popup.");
        return;
    }

    if (getChildByName(name)) {
        WRITE_WARN("Trying to add same popup twice.");
        return;
    }

    popup->showEffect();
    
    addChild(popup, zOrder, name);
    _activePopups.push(popup);
    
    WRITE_LOG("Pushed popup: " + name);
}

bool PopupCarrier::init()
{
    if (!cocos2d::Scene::init()) {
        return false;
    }
    
    scheduleUpdate();
    
    return true;
}

void PopupCarrier::update(float dt)
{
    for (auto child :getChildren()) {
        Popup *popup = dynamic_cast<Popup *>(child);
        if (popup && popup->isClosing()) {
            removePopup(popup);
            popup->setClose(false);
            WRITE_LOG("Removed popup: " + popup->getName());
        }
    }
}

void PopupCarrier::removePopup(Popup *popup)
{
    float time = popup->hideEffect();
    std::string name = popup->getName();
    
    auto func = [&]() {
        Popup *front = _activePopups.front();
        if (front) {
            WRITE_LOG("Removed popup: " + front->getName());
            removeChild(front);
            _activePopups.pop();
        }
    };
    
    auto wait = cocos2d::DelayTime::create(time);
    auto call = cocos2d::CallFunc::create(func);
    auto seq = cocos2d::Sequence::create(wait, call, nullptr);
    
    runAction(seq);
}

bool PopupCarrier::touchBegan(cocos2d::Touch *touch, cocos2d::Event *e)
{
    if (!_activePopups.empty())
    {
        Popup *popup = _activePopups.front();
        
        cocos2d::Vec2 pt;
        pt = popup->convertTouchToNodeSpace(touch);
        
        if (!popup->hitTest(pt))
        {
            removePopup(popup);
            return false;
        }
    }
    
    return true;
}
