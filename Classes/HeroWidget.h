//
//  HeroWidget.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 06/04/15.
//
//

#ifndef __SlashingTrough__HeroWidget__
#define __SlashingTrough__HeroWidget__

#include "cocos2d.h"
#include "CCMotionStreak.h"

#include "Hero.h"
#include "PathSectorWidget.h"

class GameField;

class HeroWidget : public cocos2d::Node
{
public:
    enum class SwordSide
    {
        LEFT,
        RIGHT
    };
    
public:
    static HeroWidget* create(GameplayObject::WeakPtr hero, GameField *gamefield);
    
    void RefreshSectorsSequence(PathSectorWidget::SectorsSequence &sectors);
    void RunEffectReceiveDamage();
    void RunEffectSwordTrail(float duration);
    
    GameplayObject::WeakPtr GetHero() const;
    
    SwordSide GetSwordSide() const;
    
protected:
    HeroWidget(GameplayObject::WeakPtr Hero, GameField *gamefield);
    virtual ~HeroWidget();
    
    bool init();
    void update(float dt);
    
    void Attack();
    void PerformAction(const HeroAction &action);
    
    cocos2d::FiniteTimeAction* HorizontalMotion(float deltaX, float deltaY, float time);
    cocos2d::FiniteTimeAction* HorizontalAttack(float attackTime = 0.0f);
    
    cocos2d::FiniteTimeAction* AnimSwordRightSwipeRight(float duration);
    cocos2d::FiniteTimeAction* AnimSwordRightSwipeLeft(float duration);
    cocos2d::FiniteTimeAction* AnimSwordLeftSwipeRight(float duration);
    cocos2d::FiniteTimeAction* AnimSwordLeftSwipeLeft(float duration);
    
private:
    struct SwordTransform
    {
        cocos2d::Vec2 localPos;
        float angle;
        
        SwordTransform(cocos2d::Vec2 p, float a)
        : localPos(p)
        , angle(a)
        {}
        SwordTransform()
        {}
    };

private:
    static const SwordTransform _swordRightSideTrans;
    static const SwordTransform _swordLeftSideTrans;
    
    GameField *_gamefield;
    GameplayObject::WeakPtr _hero;
    PathSectorWidget::SectorsSequence *_sectors;
    
    cocos2d::Node *_bodyControlNode; // all rotations performed by this node
    cocos2d::Sprite *_sword;
    cocos2d::DrawNode *_body;
    cocos2d::DrawNode *_bodyBorder;
    cocos2d::MotionStreak *_trail;
    
    SwordSide _swordSide;
    
    float _lastStamina;
    int _attackedRowIndex;
    bool _isGameplayActionRunning;
};

#endif /* defined(__SlashingTrough__HeroWidget__) */
