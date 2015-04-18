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
    
protected:
    EnemyWidget(GameplayObject::WeakPtr enemy);
    virtual ~EnemyWidget();
    
    bool init();
    
    GameplayObject::Ptr GetEnemy() const;
    
private:
    GameplayObject::WeakPtr _enemy;
    cocos2d::Sprite *_sprite;
    cocos2d::DrawNode *_leftFoot;
    cocos2d::DrawNode *_rightFoot;
};

#endif /* defined(__SlashingTrough__GameplayObjectsWidgets__) */
