//
//  CharacterControlTouch.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 23/04/15.
//
//

#ifndef __SlashingTrough__CharacterControlTouch__
#define __SlashingTrough__CharacterControlTouch__

#include "cocos2d.h"
#include "Character.h"

class CharacterControlTouch
{
public:
    typedef std::shared_ptr<CharacterControlTouch> Ptr;
    typedef std::weak_ptr<CharacterControlTouch> WeakPtr;
    
public:
    static CharacterControlTouch::Ptr Create(GameplayObject::WeakPtr charater,
                                                cocos2d::EventDispatcher *dispatcher,
                                                cocos2d::Node *node);
    CharacterControlTouch(GameplayObject::WeakPtr character,
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
    
    GameplayObject::WeakPtr _character;
    cocos2d::EventDispatcher *_dispatcher;
    cocos2d::EventListenerTouchOneByOne *_listener;
    cocos2d::Vec2 _touchBeginPos;
    
    bool _isSwipingNow;
};

#endif /* defined(__SlashingTrough__CharacterControlTouch__) */
