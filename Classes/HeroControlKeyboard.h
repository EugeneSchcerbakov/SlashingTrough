//
//  HeroControlKeyboard.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 07/04/15.
//
//

#ifndef __SlashingTrough__HeroControlKeyboard__
#define __SlashingTrough__HeroControlKeyboard__

#include "cocos2d.h"
#include "Hero.h"

class HeroControlKeyboard
{
public:
    typedef std::shared_ptr<HeroControlKeyboard> Ptr;
    typedef std::weak_ptr<HeroControlKeyboard> WeakPtr;
    
public:
    static HeroControlKeyboard::Ptr Create(GameplayObject::WeakPtr hero,
                                                cocos2d::EventDispatcher *dispatcher,
                                                cocos2d::Node *node);
    HeroControlKeyboard(GameplayObject::WeakPtr hero,
                             cocos2d::EventDispatcher *dispatcher,
                             cocos2d::Node *node);
    void Free();
    
private:
    bool Init(cocos2d::Node *node);
    void OnKeyPressed(cocos2d::EventKeyboard::KeyCode key, cocos2d::Event *event);
    
    const float _squareSize;
    
    GameplayObject::WeakPtr _hero;
    cocos2d::EventDispatcher *_dispatcher;
    cocos2d::EventListenerKeyboard *_listener;
};

#endif /* defined(__SlashingTrough__HeroControlKeyboard__) */
