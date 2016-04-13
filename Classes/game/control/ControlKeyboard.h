//
//  ControlKeyboard.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 07/04/15.
//
//

#ifndef __SlashingTrough__ControlKeyboard__
#define __SlashingTrough__ControlKeyboard__

#include "cocos2d.h"
#include "Hero.h"

class ControlKeyboard
{
public:
    typedef std::shared_ptr<ControlKeyboard> Ptr;
    typedef std::weak_ptr<ControlKeyboard> WeakPtr;
    
public:
    static ControlKeyboard::Ptr create(Hero *hero, cocos2d::EventDispatcher *dispatcher, cocos2d::Node *node);
    ControlKeyboard(Hero *hero, cocos2d::EventDispatcher *dispatcher, cocos2d::Node *node);
    void free();
    
private:
    bool init(cocos2d::Node *node);
    void onKeyPressed(cocos2d::EventKeyboard::KeyCode key, cocos2d::Event *event);
    
    const float _squareSize;
    
    Hero *_hero;
    cocos2d::EventDispatcher *_dispatcher;
    cocos2d::EventListenerKeyboard *_listener;
};

#endif /* defined(__SlashingTrough__HeroControlKeyboard__) */
