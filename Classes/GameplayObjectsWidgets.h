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

class ObstacleWidget : public cocos2d::Node
{
public:
    static ObstacleWidget* create(GameplayObject::WeakPtr obstacle);
    
protected:
    ObstacleWidget(GameplayObject::WeakPtr obstacle);
    virtual ~ObstacleWidget();
    
    bool init();
    
    GameplayObject::Ptr GetObstacle() const;
    
private:
    GameplayObject::WeakPtr _obstacle;
    
    cocos2d::Sprite *_sprite;
};

class EnemyWidget : public cocos2d::Node
{
public:
    static EnemyWidget* create(GameplayObject::WeakPtr enemy);
    
    void RunHitAccentEffect();
    
protected:
    EnemyWidget(GameplayObject::WeakPtr enemy);
    virtual ~EnemyWidget();
    
    bool init();
    void update(float dt);
    
    GameplayObject::Ptr GetEnemy() const;
    
private:
    GameplayObject::WeakPtr _enemy;
    HealthBarWidget *_healthWidet;
    cocos2d::Sprite *_sprite;
    
    float _lastHealth;
};

#endif /* defined(__SlashingTrough__GameplayObjectsWidgets__) */
