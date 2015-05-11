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

#include "Hero.h"
#include "PathSectorWidget.h"

class HeroWidget : public cocos2d::Node
{
public:
    static HeroWidget* create(GameplayObject::WeakPtr Hero);
    
    void RefreshSectorsSequence(PathSectorWidget::SectorsSequence &sectors);
    void RunEffectReceiveDamage();
    
    GameplayObject::WeakPtr GetHero() const;
    
protected:
    HeroWidget(GameplayObject::WeakPtr Hero);
    virtual ~HeroWidget();
    
    bool init();
    void update(float dt);
    
    void Attack();
    void PerformAction(const HeroAction &action);
    
    cocos2d::Action* AnimSwordRightSwipeRight(float duration);
    cocos2d::Action* AnimSwordRightSwipeLeft(float duration);
    cocos2d::Action* AnimSwordLeftSwipeRight(float duration);
    cocos2d::Action* AnimSwordLeftSwipeLeft(float duration);
    
private:
    enum class SwordSide
    {
        LEFT,
        RIGHT
    };
    
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
    
    GameplayObject::WeakPtr _hero;
  
    PathSectorWidget::SectorsSequence *_sectors;
    
    cocos2d::DrawNode *_sword;
    cocos2d::DrawNode *_body;
    cocos2d::DrawNode *_bodyBorder;
    
    SwordSide _swordSide;
    
    float _lastStamina;
    int _attackedRowIndex;
    bool _isGameplayActionRunning;
};

#endif /* defined(__SlashingTrough__HeroWidget__) */
