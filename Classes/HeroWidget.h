//
//  HeroWidget.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 11/07/15.
//
//

#ifndef __SlashingTrough__HeroWidget__
#define __SlashingTrough__HeroWidget__

#include "Hero.h"

#include "cocos2d.h"

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
    static HeroWidget* create(Hero *hero, cocos2d::Layer *fieldEffects);
    
protected:
    HeroWidget(Hero *hero, cocos2d::Layer *fieldEffects);
    virtual ~HeroWidget();
    
    bool init();
    void update(float dt);
	void removeAllAnimations();
    void runSwordTrailEffect(float duration);
    void runForwardDistortion(float time);
    void runSwirlDistortion(bool flipX);

    cocos2d::FiniteTimeAction* AnimSwordRightSwipeRight(float duration);
    cocos2d::FiniteTimeAction* AnimSwordRightSwipeLeft(float duration);
    cocos2d::FiniteTimeAction* AnimSwordLeftSwipeRight(float duration);
    cocos2d::FiniteTimeAction* AnimSwordLeftSwipeLeft(float duration);
	cocos2d::FiniteTimeAction* AnimBodySwipeRight(float duration);
	cocos2d::FiniteTimeAction* AnimBodySwipeLeft(float duration);
    
private:
    static const SwordTrans _swordRightTrans;
    static const SwordTrans _swordLeftTrans;
    
	void _handleEventSwipeRight(const VariablesSet& args);
	void _handleEventSwipeLeft(const VariablesSet& args);
	void _handleEventJumpBackStart(const VariablesSet& args);
	void _handleEventJumpForwardAttack(const VariablesSet& args);
	void _handleEventHideShield(const VariablesSet& args);
	void _handleEventShieldDamageReceived(const VariablesSet& args);
	void _handleEventDamageReceived(const VariablesSet& args);
    void _handleEventHitedByProjectile(const VariablesSet& args);

    Hero *_hero;
    
    SwordSide _swordSide;
	SwordSide _nextSwordSide;
    
    cocos2d::Sprite *_body;
    cocos2d::Sprite *_sword;
    cocos2d::Sprite *_shield;
    cocos2d::Node *_bodyController;
    cocos2d::MotionStreak *_swordTrail;
    
    cocos2d::Layer *_fieldEffects;
    cocos2d::Sprite *_dashDistor;
};

#endif /* defined(__SlashingTrough__HeroWidget__) */
