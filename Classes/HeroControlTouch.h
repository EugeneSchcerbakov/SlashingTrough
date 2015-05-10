//
//  HeroControlTouch.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 23/04/15.
//
//

#ifndef __SlashingTrough__HeroControlTouch__
#define __SlashingTrough__HeroControlTouch__

#include "cocos2d.h"
#include "Hero.h"

class HeroControlTouch
{
public:
    typedef std::shared_ptr<HeroControlTouch> Ptr;
    typedef std::weak_ptr<HeroControlTouch> WeakPtr;
    
public:
    static HeroControlTouch::Ptr Create(GameplayObject::WeakPtr hero,
                                                cocos2d::EventDispatcher *dispatcher,
                                                cocos2d::Node *node);
    HeroControlTouch(GameplayObject::WeakPtr Hero,
                             cocos2d::EventDispatcher *dispatcher,
                             cocos2d::Node *node);
    void Free();
    
private:
    bool Init(cocos2d::Node *node);
    bool TouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void TouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
    void TouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    
    const float _squareSize;
    const float _swipeDistance;
    
    GameplayObject::WeakPtr _hero;
    cocos2d::EventDispatcher *_dispatcher;
    cocos2d::EventListenerTouchOneByOne *_listener;
    cocos2d::Vec2 _touchBeginPos;
    
    bool _isSwipingNow;
};

#endif /* defined(__SlashingTrough__HeroControlTouch__) */
