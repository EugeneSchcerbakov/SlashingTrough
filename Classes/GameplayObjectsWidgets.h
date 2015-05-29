//
//  GameplayObjectsWidgets.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 12/04/15.
//
//

#ifndef __SlashingTrough__GameplayObjectsWidgets__
#define __SlashingTrough__GameplayObjectsWidgets__

#include "cocos2d.h"
#include "GameplayObjects.h"
#include "HeroWidget.h"

class HealthBarWidget : public cocos2d::Node
{
public:
    static HealthBarWidget* create();
    
    void Refresh(int healthPoints);
    
protected:
    HealthBarWidget();
    virtual ~HealthBarWidget();
    
    bool init();
    
private:
    cocos2d::DrawNode *_bar;
    
    int _healthPoints;
};

class GameplayObjectWidget : public cocos2d::Node
{
public:
    GameplayObjectWidget();
    virtual ~GameplayObjectWidget();
    
    virtual bool init();
    virtual void OnDamageReceived(HeroWidget::SwordSide side);
};

class ObstacleWidget : public GameplayObjectWidget
{
public:
    static ObstacleWidget* create(GameplayObject::WeakPtr obstacle);
    
protected:
    ObstacleWidget(GameplayObject::WeakPtr obstacle);
    virtual ~ObstacleWidget();
    
    bool init() override;
    
    GameplayObject::Ptr GetObstacle() const;
    
private:
    GameplayObject::WeakPtr _obstacle;
    
    cocos2d::Sprite *_sprite;
};

class EnemyWidget : public GameplayObjectWidget
{
public:
    static EnemyWidget* create(GameplayObject::WeakPtr enemy);
    
    void RunHitAccentEffect();
    void OnDamageReceived(HeroWidget::SwordSide side) override;
    
protected:
    EnemyWidget(GameplayObject::WeakPtr enemy);
    virtual ~EnemyWidget();
    
    bool init() override;
    
    GameplayObject::Ptr GetEnemy() const;
    
private:
    GameplayObject::WeakPtr _enemy;
    HealthBarWidget *_healthWidet;
    cocos2d::Sprite *_sprite;
    cocos2d::Sprite *_blood;
    cocos2d::ProgressTimer *_bloodClip;
};

#endif /* defined(__SlashingTrough__GameplayObjectsWidgets__) */
