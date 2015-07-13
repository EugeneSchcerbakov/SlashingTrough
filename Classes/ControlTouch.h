//
//  HeroControlTouch.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 23/04/15.
//
//

#ifndef __SlashingTrough__ControlTouch__
#define __SlashingTrough__ControlTouch__

#include "cocos2d.h"
#include "Hero.h"

class ControlTouch
{
public:
    typedef std::shared_ptr<ControlTouch> Ptr;
    typedef std::weak_ptr<ControlTouch> WeakPtr;
    
public:
    static ControlTouch::Ptr create(Hero *hero, cocos2d::EventDispatcher *dispatcher, cocos2d::Node *node);
    ControlTouch(Hero *Hero, cocos2d::EventDispatcher *dispatcher, cocos2d::Node *node);
    void free();
    
private:
    bool init(cocos2d::Node *node);
    bool touchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void touchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void touchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    
    const float _squareSize;
    const float _swipeDistance;
    
    Hero *_hero;
    cocos2d::EventDispatcher *_dispatcher;
    cocos2d::EventListenerTouchOneByOne *_listener;
    cocos2d::Vec2 _touchBeginPos;
    
    bool _isSwipingNow;
};

#endif /* defined(__SlashingTrough__HeroControlTouch__) */
