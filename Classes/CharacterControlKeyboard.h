//
//  CharacterControlKeyboard.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 07/04/15.
//
//

#ifndef __SlashingTrough__CharacterControlKeyboard__
#define __SlashingTrough__CharacterControlKeyboard__

#include "cocos2d.h"
#include "Character.h"

class CharacterControlKeyboard
{
public:
    typedef std::shared_ptr<CharacterControlKeyboard> Ptr;
    typedef std::weak_ptr<CharacterControlKeyboard> WeakPtr;
    
public:
    static CharacterControlKeyboard::Ptr Create(GameplayObject::WeakPtr charater,
                                                cocos2d::EventDispatcher *dispatcher,
                                                cocos2d::Node *node);
    CharacterControlKeyboard(GameplayObject::WeakPtr character,
                             cocos2d::EventDispatcher *dispatcher,
                             cocos2d::Node *node);
    void Free();
    
private:
    bool Init(cocos2d::Node *node);
    void OnKeyPressed(cocos2d::EventKeyboard::KeyCode key, cocos2d::Event *event);
    
    const float _squareSize;
    
    GameplayObject::WeakPtr _character;
    cocos2d::EventDispatcher *_dispatcher;
    cocos2d::EventListenerKeyboard *_listener;
};

#endif /* defined(__SlashingTrough__CharacterControlKeyboard__) */
