//
//  CharacterControlTouch.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 23/04/15.
//
//

#include "CharacterControlTouch.h"

CharacterControlTouch::Ptr CharacterControlTouch::Create(GameplayObject::WeakPtr charater,
                                         cocos2d::EventDispatcher *dispatcher,
                                         cocos2d::Node *node)
{
    return std::make_shared<CharacterControlTouch>(charater, dispatcher, node);
}

CharacterControlTouch::CharacterControlTouch(GameplayObject::WeakPtr character,
                      cocos2d::EventDispatcher *dispatcher,
                      cocos2d::Node *node)
: _character(character)
, _dispatcher(dispatcher)
, _listener(nullptr)
, _squareSize(GameInfo::Instance().GetFloat("SQUARE_SIZE"))
, _swipeDistance(GameInfo::Instance().GetFloat("TOUCH_SWIPE_DISTANCE"))
, _isSwipingNow(false)
{
    Init(node);
}

void CharacterControlTouch::Free()
{
    if (_dispatcher) {
        _dispatcher->removeEventListener(_listener);
    }
}

bool CharacterControlTouch::Init(cocos2d::Node *node)
{
    if (!_dispatcher || _character.expired()) {
        return false;
    }
    
    _listener = cocos2d::EventListenerTouchOneByOne::create();
    _listener->onTouchBegan = CC_CALLBACK_2(CharacterControlTouch::TouchBegan, this);
    _listener->onTouchMoved = CC_CALLBACK_2(CharacterControlTouch::TouchMoved, this);
    _listener->onTouchEnded = CC_CALLBACK_2(CharacterControlTouch::TouchEnded, this);
    _listener->onTouchCancelled = CC_CALLBACK_2(CharacterControlTouch::TouchEnded, this);
    _dispatcher->addEventListenerWithSceneGraphPriority(_listener, node);
    
    return true;
}

bool CharacterControlTouch::TouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    _touchBeginPos = touch->getLocation();
    _isSwipingNow = true;
    return true;
}

void CharacterControlTouch::TouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
    if (!_isSwipingNow || _character.expired()) {
        return;
    }
    
    GameplayObject::Ptr characterPtr = _character.lock();
    Character *character = Character::Cast(characterPtr);
    
    if (character->IsActionsQueueFull()) {
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
            const float deltaY = GameInfo::Instance().GetFloat("CHARACTER_SWIPE_OFFSET_ON_SQUARE") * _squareSize;
            const float duration = GameInfo::Instance().GetFloat("CHARACTER_SWIPE_DURATION");
            CharacterAction::Type type = CharacterAction::Type::SWIPE_RIGHT;
            CharacterAction action(type, duration, deltaX, deltaY);
            if (character->IsAbleToPerform(action)) {
                character->AddAction(action);
                _touchBeginPos = location;
                _isSwipingNow = false;
            }
        }
        if (scalar <= -0.8f)
        {
            const float deltaX = -_squareSize;
            const float deltaY = GameInfo::Instance().GetFloat("CHARACTER_SWIPE_OFFSET_ON_SQUARE") * _squareSize;
            const float duration = GameInfo::Instance().GetFloat("CHARACTER_SWIPE_DURATION");
            CharacterAction::Type type = CharacterAction::Type::SWIPE_LEFT;
            CharacterAction action(type, duration, deltaX, deltaY);
            if (character->IsAbleToPerform(action)) {
                character->AddAction(action);
                _touchBeginPos = location;
                _isSwipingNow = false;
            }
        }
    }
}

void CharacterControlTouch::TouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    _isSwipingNow = false;
}
