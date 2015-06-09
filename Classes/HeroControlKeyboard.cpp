//
//  HeroControlKeyboard.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 07/04/15.
//
//

#include "HeroControlKeyboard.h"

#include "GameInfo.h"

HeroControlKeyboard::Ptr HeroControlKeyboard::Create(GameplayObject::WeakPtr hero,
                                                               cocos2d::EventDispatcher *dispatcher,
                                                               cocos2d::Node *node)
{
    return std::make_shared<HeroControlKeyboard>(hero, dispatcher, node);
}

HeroControlKeyboard::HeroControlKeyboard(GameplayObject::WeakPtr hero,
                                                   cocos2d::EventDispatcher *dispatcher,
                                                   cocos2d::Node *node)
: _hero(hero)
, _dispatcher(dispatcher)
, _listener(nullptr)
, _squareSize(GameInfo::Instance().GetFloat("SQUARE_SIZE"))
{
    Init(node);
}

void HeroControlKeyboard::OnKeyPressed(cocos2d::EventKeyboard::KeyCode key, cocos2d::Event *event)
{
    if (_hero.expired()) {
        return;
    }
    
    GameplayObject::Ptr heroPtr = _hero.lock();
    Hero *hero = Hero::Cast(heroPtr);
    
    if (hero->IsActionsQueueFull()) {
        return;
    }
    
    if (key == cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW) {
        const float deltaX = _squareSize;
        const float deltaY = GameInfo::Instance().GetFloat("HERO_SWIPE_OFFSET_ON_SQUARE") * _squareSize;
        const float duration = hero->GetWeapon()->speed;
        HeroAction::Type type = HeroAction::Type::SWIPE_RIGHT;
        HeroAction action(type, duration, deltaX, deltaY);
        if (hero->IsAbleToPerform(action)) {
            hero->AddAction(action);
        }
    }
    if (key == cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW) {
        const float deltaX = -_squareSize;
        const float deltaY = GameInfo::Instance().GetFloat("HERO_SWIPE_OFFSET_ON_SQUARE") * _squareSize;
        const float duration = hero->GetWeapon()->speed;
        HeroAction::Type type = HeroAction::Type::SWIPE_LEFT;
        HeroAction action(type, duration, deltaX, deltaY);
        if (hero->IsAbleToPerform(action)) {
            hero->AddAction(action);
        }
    }
    if (key == cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW) {
        float deltaY = GameInfo::Instance().GetFloat("HERO_JUMP_BACK_DISTANCE");
        float duration = GameInfo::Instance().GetFloat("HERO_JUMP_BACK_DURATION");
        HeroAction::Type type = HeroAction::Type::JUMP_BACK;
        HeroAction action(type, duration, 0.0f, deltaY);
        if (hero->IsAbleToPerform(action)) {
            hero->AddAction(action);
        }
    }
}

bool HeroControlKeyboard::Init(cocos2d::Node *node)
{
    if (!_dispatcher || _hero.expired()) {
        return false;
    }
    
    _listener = cocos2d::EventListenerKeyboard::create();
    _listener->onKeyPressed = CC_CALLBACK_2(HeroControlKeyboard::OnKeyPressed, this);
    _dispatcher->addEventListenerWithSceneGraphPriority(_listener, node);
    
    return true;
}

void HeroControlKeyboard::Free()
{
    if (_dispatcher) {
        _dispatcher->removeEventListener(_listener);
    }
}
