//
//  CharacterControlKeyboard.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 07/04/15.
//
//

#include "CharacterControlKeyboard.h"

#include "GameInfo.h"

CharacterControlKeyboard::Ptr CharacterControlKeyboard::Create(GameplayObject::WeakPtr charater,
                                                               cocos2d::EventDispatcher *dispatcher,
                                                               cocos2d::Node *node)
{
    return std::make_shared<CharacterControlKeyboard>(charater, dispatcher, node);
}

CharacterControlKeyboard::CharacterControlKeyboard(GameplayObject::WeakPtr character,
                                                   cocos2d::EventDispatcher *dispatcher,
                                                   cocos2d::Node *node)
: _character(character)
, _dispatcher(dispatcher)
, _listener(nullptr)
, _squareSize(GameInfo::Instance().GetFloat("SQUARE_SIZE"))
{
    Init(node);
}

void CharacterControlKeyboard::OnKeyPressed(cocos2d::EventKeyboard::KeyCode key, cocos2d::Event *event)
{
    if (_character.expired()) {
        return;
    }
    
    GameplayObject::Ptr characterPtr = _character.lock();
    Character *character = Character::Cast(characterPtr);
    
    if (character->IsActionsQueueFull()) {
        return;
    }
    
    if (key == cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW) {
        const float deltaX = _squareSize;
        const float deltaY = GameInfo::Instance().GetFloat("CHARACTER_SWIPE_OFFSET_ON_SQUARE") * _squareSize;
        const float duration = GameInfo::Instance().GetFloat("CHARACTER_SWIPE_DURATION");
        CharacterAction::Type type = CharacterAction::Type::SWIPE_RIGHT;
        CharacterAction action(type, duration, deltaX, deltaY);
        if (character->IsAbleToPerform(action)) {
            character->AddAction(action);
        }
    }
    if (key == cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW) {
        const float deltaX = -_squareSize;
        const float deltaY = GameInfo::Instance().GetFloat("CHARACTER_SWIPE_OFFSET_ON_SQUARE") * _squareSize;
        const float duration = GameInfo::Instance().GetFloat("CHARACTER_SWIPE_DURATION");
        CharacterAction::Type type = CharacterAction::Type::SWIPE_LEFT;
        CharacterAction action(type, duration, deltaX, deltaY);
        if (character->IsAbleToPerform(action)) {
            character->AddAction(action);
        }
    }
}

bool CharacterControlKeyboard::Init(cocos2d::Node *node)
{
    if (!_dispatcher || _character.expired()) {
        return false;
    }
    
    _listener = cocos2d::EventListenerKeyboard::create();
    _listener->onKeyPressed = CC_CALLBACK_2(CharacterControlKeyboard::OnKeyPressed, this);
    _dispatcher->addEventListenerWithSceneGraphPriority(_listener, node);
    
    return true;
}

void CharacterControlKeyboard::Free()
{
    if (_dispatcher) {
        _dispatcher->removeEventListener(_listener);
    }
}
