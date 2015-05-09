//
//  GameplayObjectsWidgets.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 12/04/15.
//
//

#include "GameplayObjectsWidgets.h"

// HealthBarWidget implementation

HealthBarWidget* HealthBarWidget::create()
{
    HealthBarWidget *widget = new HealthBarWidget();
    if (widget && widget->init()) {
        widget->autorelease();
    } else {
        delete widget;
        widget = nullptr;
    }
    return widget;
}

HealthBarWidget::HealthBarWidget()
: _healthPoints(0)
{
}

HealthBarWidget::~HealthBarWidget()
{
}

bool HealthBarWidget::init()
{
    if (!cocos2d::Node::init()) {
        return false;
    }
    
    _bar = cocos2d::DrawNode::create();
    addChild(_bar);
    
    return true;
}

void HealthBarWidget::Refresh(int healthPoints)
{
    _healthPoints = healthPoints;
    
    cocos2d::Color4F barColor(0.75f, 0.0f, 0.0f, 1.0f);
    
    float radius = 15.0f;
    float w = radius * 2.0f;
    float shiftX = 10.0f;
    float totalWidth = w * healthPoints + shiftX * (healthPoints - 1);
    float offset = (totalWidth - w) * 0.5f;
    
    _bar->clear();
    
    for (int k = 0; k < _healthPoints; ++k) {
        float w = radius * 2.0f;
        float x = w * k + shiftX * k;
        _bar->drawSolidCircle(cocos2d::Vec2(x - offset, 0.0f), radius, 0.0f, 10, 1.0f, 1.0f, barColor);
    }
}

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
    
    _lastHealth = ptr->GetHealth();
    
    _sprite = cocos2d::Sprite::create(ptr->GetSpriteFilename());
    _sprite->setPosition(0.0f, 0.0f);
    
    float healthWidgetYShift = 80.0f;
    _healthWidet = HealthBarWidget::create();
    _healthWidet->setPositionY(healthWidgetYShift);
    _healthWidet->Refresh((int)ptr->GetHealth());
    
    addChild(_sprite, 0);
    addChild(_healthWidet, 1);
    scheduleUpdate();
   
    return true;
}

void EnemyWidget::update(float dt)
{
    GameplayObject::Ptr ptr = _enemy.lock();
    
    if (_lastHealth != ptr->GetHealth()) {
        _lastHealth = ptr->GetHealth();
        _healthWidet->Refresh((int)_lastHealth);
        RunHitAccentEffect();
    }
}

void EnemyWidget::RunHitAccentEffect()
{
    cocos2d::ScaleTo *scale0 = cocos2d::ScaleTo::create(0.1f, 0.9f);
    cocos2d::ScaleTo *scale1 = cocos2d::ScaleTo::create(0.1f, 1.0f);
    cocos2d::Sequence *effect = cocos2d::Sequence::create(scale0, scale1, nullptr);
    _sprite->runAction(effect);
}

GameplayObject::Ptr EnemyWidget::GetEnemy() const
{
    return _enemy.lock();
}
