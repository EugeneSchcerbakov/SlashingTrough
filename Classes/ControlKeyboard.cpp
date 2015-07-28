//
//  ControlKeyboard.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 07/04/15.
//
//

#include "ControlKeyboard.h"

#include "GameInfo.h"

ControlKeyboard::Ptr ControlKeyboard::create(Hero *hero, cocos2d::EventDispatcher *dispatcher, cocos2d::Node *node)
{
    return std::make_shared<ControlKeyboard>(hero, dispatcher, node);
}

ControlKeyboard::ControlKeyboard(Hero *hero, cocos2d::EventDispatcher *dispatcher, cocos2d::Node *node)
: _hero(hero)
, _dispatcher(dispatcher)
, _listener(nullptr)
, _squareSize(GameInfo::getInstance().getFloat("SQUARE_SIZE"))
{
    init(node);
}

void ControlKeyboard::onKeyPressed(cocos2d::EventKeyboard::KeyCode key, cocos2d::Event *event)
{
    if (!_hero) {
        return;
    }
    
    if (_hero->isActionsQueueFull()) {
        return;
    }
    
    GameInfo &gameinfo = GameInfo::getInstance();
    
    if (key == cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW) {
        const float deltaX = _squareSize;
        const float deltaY = gameinfo.getFloat("HERO_SWIPE_OFFSET_ON_SQUARE") * _squareSize;
        const float duration = _hero->getWeapon() ? _hero->getWeapon()->speed : 0.0f;
        
        HeroAction *action = new AttackAndMove(_hero, duration, deltaX, deltaY);
        if (_hero->isAbleToPerform(action))
        {
            Event e("SwipeRight");
            e.variables.setFloat("duration", duration);
            action->setEvent(e);
            
            _hero->addAction(action);
        }
    }
    if (key == cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW) {
        const float deltaX = -_squareSize;
        const float deltaY = gameinfo.getFloat("HERO_SWIPE_OFFSET_ON_SQUARE") * _squareSize;
        const float duration = _hero->getWeapon() ? _hero->getWeapon()->speed : 0.0f;
        
        HeroAction *action = new AttackAndMove(_hero, duration, deltaX, deltaY);
        if (_hero->isAbleToPerform(action))
        {
            Event e("SwipeLeft");
            e.variables.setFloat("duration", duration);
            action->setEvent(e);
            
            _hero->addAction(action);
        }
    }
    if (key == cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW) {
        float deltaY = gameinfo.getFloat("HERO_JUMP_BACK_DISTANCE");
        float duration = gameinfo.getFloat("HERO_JUMP_BACK_DURATION");
        
        HeroAction *action = new JumpBack(_hero, duration, -deltaY);
        if (_hero->isAbleToPerform(action))
        {
            Event e("JumpBack");
            e.variables.setFloat("duration", duration);
            action->setEvent(e);
            
            _hero->addAction(action);
        }
    }
}

bool ControlKeyboard::init(cocos2d::Node *node)
{
    if (!_dispatcher || !_hero) {
        return false;
    }
    
    _listener = cocos2d::EventListenerKeyboard::create();
    _listener->onKeyPressed = CC_CALLBACK_2(ControlKeyboard::onKeyPressed, this);
    _dispatcher->addEventListenerWithSceneGraphPriority(_listener, node);
    
    return true;
}

void ControlKeyboard::free()
{
    if (_dispatcher) {
        _dispatcher->removeEventListener(_listener);
    }
}

