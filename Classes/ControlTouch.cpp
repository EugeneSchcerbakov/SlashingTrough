//
//  HeroControlTouch.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 23/04/15.
//
//

#include "ControlTouch.h"
#include "GameInfo.h"

ControlTouch::Ptr ControlTouch::create(Hero *hero, cocos2d::EventDispatcher *dispatcher, cocos2d::Node *node)
{
    return std::make_shared<ControlTouch>(hero, dispatcher, node);
}

ControlTouch::ControlTouch(Hero *hero, cocos2d::EventDispatcher *dispatcher, cocos2d::Node *node)
: _hero(hero)
, _dispatcher(dispatcher)
, _listener(nullptr)
, _squareSize(GameInfo::getInstance().getConstFloat("SQUARE_SIZE"))
, _swipeDistance(GameInfo::getInstance().getConstFloat("TOUCH_SWIPE_DISTANCE"))
, _isSwipingNow(false)
{
    init(node);
}

void ControlTouch::free()
{
    if (_dispatcher) {
        _dispatcher->removeEventListener(_listener);
    }
}

bool ControlTouch::init(cocos2d::Node *node)
{
    if (!_dispatcher || !_hero) {
        return false;
    }
    
    _listener = cocos2d::EventListenerTouchOneByOne::create();
    _listener->onTouchBegan = CC_CALLBACK_2(ControlTouch::touchBegan, this);
    _listener->onTouchMoved = CC_CALLBACK_2(ControlTouch::touchMoved, this);
    _listener->onTouchEnded = CC_CALLBACK_2(ControlTouch::touchEnded, this);
    _listener->onTouchCancelled = CC_CALLBACK_2(ControlTouch::touchEnded, this);
    _dispatcher->addEventListenerWithSceneGraphPriority(_listener, node);
    
    return true;
}

bool ControlTouch::touchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    _touchBeginPos = touch->getLocation();
    _isSwipingNow = true;
    return true;
}

void ControlTouch::touchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
    if (!_isSwipingNow || !_hero) {
        return;
    }
    
    if (_hero->isActionsQueueFull()) {
        return;
    }
    
    GameInfo &gameinfo = GameInfo::getInstance();
    
    cocos2d::Vec2 location = touch->getLocation();
    float d = location.distance(_touchBeginPos);
    
    if (d > _swipeDistance)
    {
        cocos2d::Vec2 v0 = (location - _touchBeginPos).getNormalized();
        cocos2d::Vec2 v1 = cocos2d::Vec2::UNIT_X;
        const float scalar = v0.x * v1.x + v0.y * v1.y;
        
        if (scalar >= 0.8) {
            const float deltaX = _squareSize;
            const float deltaY = gameinfo.getConstFloat("HERO_SWIPE_OFFSET_ON_SQUARE") * _squareSize;
            const float duration = _hero->getWeapon() ? _hero->getWeapon()->getSpeed() : 0.0f;
            
            HeroAction *action = new AttackAndMove(_hero, duration, deltaX, deltaY);
            if (_hero->isAbleToPerform(action))
            {
                Event e("SwipeRight");
                e.variables.setFloat("duration", duration);
                action->setEvent(e);
                
                _hero->addAction(action);
                _isSwipingNow = false;
            }
            _hero->onSwipeRight();
        }
        if (scalar <= -0.8f)
        {
            const float deltaX = -_squareSize;
            const float deltaY = gameinfo.getConstFloat("HERO_SWIPE_OFFSET_ON_SQUARE") * _squareSize;
            const float duration = _hero->getWeapon() ? _hero->getWeapon()->getSpeed() : 0.0f;
            
            HeroAction *action = new AttackAndMove(_hero, duration, deltaX, deltaY);
            if (_hero->isAbleToPerform(action))
            {
                Event e("SwipeLeft");
                e.variables.setFloat("duration", duration);
                action->setEvent(e);
                
                _hero->addAction(action);
                _isSwipingNow = false;
            }
            _hero->onSwipeLeft();
        }
        // swipe down
        if (scalar >= -0.3f && scalar <= 0.3 && touch->getDelta().y < 0.0f) {
            _hero->onSwipeBack();
        }
    }
}

void ControlTouch::touchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    _isSwipingNow = false;
}
