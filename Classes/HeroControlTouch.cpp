//
//  HeroControlTouch.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 23/04/15.
//
//

#include "HeroControlTouch.h"

HeroControlTouch::Ptr HeroControlTouch::Create(GameplayObject::WeakPtr hero,
                                         cocos2d::EventDispatcher *dispatcher,
                                         cocos2d::Node *node)
{
    return std::make_shared<HeroControlTouch>(hero, dispatcher, node);
}

HeroControlTouch::HeroControlTouch(GameplayObject::WeakPtr hero,
                      cocos2d::EventDispatcher *dispatcher,
                      cocos2d::Node *node)
: _hero(hero)
, _dispatcher(dispatcher)
, _listener(nullptr)
, _squareSize(GameInfo::Instance().GetFloat("SQUARE_SIZE"))
, _swipeDistance(GameInfo::Instance().GetFloat("TOUCH_SWIPE_DISTANCE"))
, _isSwipingNow(false)
{
    Init(node);
}

void HeroControlTouch::Free()
{
    if (_dispatcher) {
        _dispatcher->removeEventListener(_listener);
    }
}

bool HeroControlTouch::Init(cocos2d::Node *node)
{
    if (!_dispatcher || _hero.expired()) {
        return false;
    }
    
    _listener = cocos2d::EventListenerTouchOneByOne::create();
    _listener->onTouchBegan = CC_CALLBACK_2(HeroControlTouch::TouchBegan, this);
    _listener->onTouchMoved = CC_CALLBACK_2(HeroControlTouch::TouchMoved, this);
    _listener->onTouchEnded = CC_CALLBACK_2(HeroControlTouch::TouchEnded, this);
    _listener->onTouchCancelled = CC_CALLBACK_2(HeroControlTouch::TouchEnded, this);
    _dispatcher->addEventListenerWithSceneGraphPriority(_listener, node);
    
    return true;
}

bool HeroControlTouch::TouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    _touchBeginPos = touch->getLocation();
    _isSwipingNow = true;
    return true;
}

void HeroControlTouch::TouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
    if (!_isSwipingNow || _hero.expired()) {
        return;
    }
    
    GameplayObject::Ptr heroPtr = _hero.lock();
    Hero *hero = Hero::Cast(heroPtr);
    
    if (hero->IsActionsQueueFull()) {
        return;
    }
    
    cocos2d::Vec2 location = touch->getLocation();
    
    float D = location.distance(_touchBeginPos);
    if (D > _swipeDistance)
    {
        cocos2d::Vec2 v0 = (location - _touchBeginPos).getNormalized();
        cocos2d::Vec2 v1 = cocos2d::Vec2::UNIT_X;
        const float scalar = v0.x * v1.x + v0.y * v1.y;
        
        if (scalar >= 0.8)
        {
            const float deltaX = _squareSize;
            const float deltaY = GameInfo::Instance().GetFloat("HERO_SWIPE_OFFSET_ON_SQUARE") * _squareSize;
            const float duration = GameInfo::Instance().GetFloat("HERO_SWIPE_DURATION");
            HeroAction::Type type = HeroAction::Type::SWIPE_RIGHT;
            HeroAction action(type, duration, deltaX, deltaY);
            if (hero->IsAbleToPerform(action)) {
                hero->AddAction(action);
                _touchBeginPos = location;
                _isSwipingNow = false;
            }
        }
        if (scalar <= -0.8f)
        {
            const float deltaX = -_squareSize;
            const float deltaY = GameInfo::Instance().GetFloat("HERO_SWIPE_OFFSET_ON_SQUARE") * _squareSize;
            const float duration = GameInfo::Instance().GetFloat("HERO_SWIPE_DURATION");
            HeroAction::Type type = HeroAction::Type::SWIPE_LEFT;
            HeroAction action(type, duration, deltaX, deltaY);
            if (hero->IsAbleToPerform(action)) {
                hero->AddAction(action);
                _touchBeginPos = location;
                _isSwipingNow = false;
            }
        }
        // swipe down
        if (scalar >= -0.3f && scalar <= 0.3 && touch->getDelta().y < 0.0f) {
            float deltaY = GameInfo::Instance().GetFloat("HERO_JUMP_BACK_DISTANCE");
            float duration = GameInfo::Instance().GetFloat("HERO_JUMP_BACK_DURATION");
            HeroAction::Type type = HeroAction::Type::JUMP_BACK;
            HeroAction action(type, duration, 0.0f, deltaY);
            if (hero->IsAbleToPerform(action)) {
                hero->AddAction(action);
                _touchBeginPos = location;
                _isSwipingNow = false;
            }
        }
    }
}

void HeroControlTouch::TouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    _isSwipingNow = false;
}
