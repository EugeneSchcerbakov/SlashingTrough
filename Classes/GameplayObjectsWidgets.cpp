//
//  GameplayObjectsWidgets.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 12/04/15.
//
//

#include "GameplayObjectsWidgets.h"

// ObstacleWidget implementation

ObstacleWidget* ObstacleWidget::create(GameplayObject::WeakPtr obstacle)
{
    ObstacleWidget *widget = new ObstacleWidget(obstacle);
    if (widget && widget->init()) {
        widget->autorelease();
    } else {
        delete widget;
        widget = nullptr;
    }
    return widget;
}

ObstacleWidget::ObstacleWidget(GameplayObject::WeakPtr obstacle)
: _obstacle(obstacle)
{
}

ObstacleWidget::~ObstacleWidget()
{
}

bool ObstacleWidget::init()
{
    if (!cocos2d::Node::init()) {
        return false;
    }
    
    GameplayObject::Ptr ptr = _obstacle.lock();
    
    _sprite = cocos2d::Sprite::create(ptr->GetSpriteFilename());
    _sprite->setPosition(0.0f, 0.0f);
    
    addChild(_sprite);
    
    return true;
}

GameplayObject::Ptr ObstacleWidget::GetObstacle() const
{
    return _obstacle.lock();
}

// Enemy implementation

EnemyWidget* EnemyWidget::create(GameplayObject::WeakPtr enemy)
{
    EnemyWidget *widget = new EnemyWidget(enemy);
    if (widget && widget->init()) {
        widget->autorelease();
    } else {
        delete widget;
        widget = nullptr;
    }
    return widget;
}

EnemyWidget::EnemyWidget(GameplayObject::WeakPtr enemy)
: _enemy(enemy)
{
}

EnemyWidget::~EnemyWidget()
{
    
}

bool EnemyWidget::init()
{
    if (!cocos2d::Node::init()) {
        return false;
    }
    
    GameplayObject::Ptr ptr = _enemy.lock();
    
    _leftFoot = cocos2d::DrawNode::create();
    _leftFoot->setPosition(cocos2d::Vec2(-30.0f, 16.0f));
    _leftFoot->drawSolidCircle(cocos2d::Vec2(0.0f, 0.0f), 25.0f, 0.0f, 20, 1.0f, 1.0f, cocos2d::Color4F::BLACK);
    
    _rightFoot = cocos2d::DrawNode::create();
    _rightFoot->setPosition(cocos2d::Vec2(30.0f, -16.0f));
    _rightFoot->drawSolidCircle(cocos2d::Vec2(0.0f, 0.0f), 25.0f, 0.0f, 20, 1.0f, 1.0f, cocos2d::Color4F::BLACK);
    
    const float stepDelta = 32.0f;
    const float stepTime = 0.5f;
    
    cocos2d::MoveBy *leftFootDown = cocos2d::MoveBy::create(stepTime, cocos2d::Vec2(0.0f, -stepDelta));
    cocos2d::MoveBy *leftFootUp = cocos2d::MoveBy::create(stepTime, cocos2d::Vec2(0.0f, stepDelta));
    cocos2d::EaseSineInOut *leftFootDownEase = cocos2d::EaseSineInOut::create(leftFootDown);
    cocos2d::EaseSineInOut *leftFootUpEase = cocos2d::EaseSineInOut::create(leftFootUp);
    cocos2d::Sequence *leftFootStep = cocos2d::Sequence::create(leftFootDownEase, leftFootUpEase, nullptr);
    cocos2d::RepeatForever *leftFootEffect = cocos2d::RepeatForever::create(leftFootStep);
    
    cocos2d::MoveBy *rightFootDown = cocos2d::MoveBy::create(stepTime, cocos2d::Vec2(0.0f, -stepDelta));
    cocos2d::MoveBy *rightFootUp = cocos2d::MoveBy::create(stepTime, cocos2d::Vec2(0.0f, stepDelta));
    cocos2d::EaseSineInOut *rightFootDownEase = cocos2d::EaseSineInOut::create(rightFootDown);
    cocos2d::EaseSineInOut *rightFootUpEase = cocos2d::EaseSineInOut::create(rightFootUp);
    cocos2d::Sequence *rightFootStep = cocos2d::Sequence::create(rightFootUpEase, rightFootDownEase, nullptr);
    cocos2d::RepeatForever *rightFootEffect = cocos2d::RepeatForever::create(rightFootStep);
    
    _leftFoot->runAction(leftFootEffect);
    _rightFoot->runAction(rightFootEffect);
    
    cocos2d::Node *footsHolder = cocos2d::Node::create();
    footsHolder->setPosition(-10.0f, -20.0f);
    footsHolder->addChild(_leftFoot);
    footsHolder->addChild(_rightFoot);
    
    _sprite = cocos2d::Sprite::create(ptr->GetSpriteFilename());
    _sprite->setPosition(0.0f, 0.0f);
    
    addChild(_sprite, 1);
    addChild(footsHolder, 0);
    
    return true;
}

GameplayObject::Ptr EnemyWidget::GetEnemy() const
{
    return _enemy.lock();
}
