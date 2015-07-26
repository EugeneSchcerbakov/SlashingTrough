//
//  HeroWidget.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 11/07/15.
//
//

#ifndef __SlashingTrough__HeroWidget__
#define __SlashingTrough__HeroWidget__

#include <cocos2d.h>

#include "Hero.h"

enum class SwordSide {
    LEFT,
    RIGHT
};

struct SwordTrans
{
    cocos2d::Vec2 pos;
    float angle;
    SwordTrans(cocos2d::Vec2 p, float a)
    : pos(p)
    , angle(a)
    {}
};

class HeroWidget : public cocos2d::BillBoard
{
public:
    static HeroWidget* create(Hero *hero);
    
protected:
    HeroWidget(Hero *hero);
    virtual ~HeroWidget();
    
    bool init();
    void update(float dt);
	void removeAllAnimations();
    void runSwordTrailEffect(float duration);
    void acceptEvent(const Event &event);
    
    cocos2d::FiniteTimeAction* AnimSwordRightSwipeRight(float duration);
    cocos2d::FiniteTimeAction* AnimSwordRightSwipeLeft(float duration);
    cocos2d::FiniteTimeAction* AnimSwordLeftSwipeRight(float duration);
    cocos2d::FiniteTimeAction* AnimSwordLeftSwipeLeft(float duration);
	cocos2d::FiniteTimeAction* AnimBodySwipeRight(float duration);
	cocos2d::FiniteTimeAction* AnimBodySwipeLeft(float duration);
    
private:
    static const SwordTrans _swordRightTrans;
    static const SwordTrans _swordLeftTrans;
    
    static void accepter(const Event &event, void *param);
    
    Hero *_hero;
    
    SwordSide _swordSide;
	SwordSide _nextSwordSide;
    
    cocos2d::Sprite *_body;
    cocos2d::Sprite *_sword;
    cocos2d::Node *_bodyController;
    cocos2d::MotionStreak *_swordTrail;
};

#endif /* defined(__SlashingTrough__HeroWidget__) */
