//
//  StaminaBarWidget.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 11/05/15.
//
//

#include "StaminaBarWidget.h"

StaminaBarWidget* StaminaBarWidget::create()
{
    StaminaBarWidget *widget = new StaminaBarWidget();
    if (widget && widget->init()) {
        widget->autorelease();
    } else {
        delete widget;
        widget = nullptr;
    }
    return widget;
}

StaminaBarWidget::StaminaBarWidget()
: _logicalProgress(1.0f)
, _visibleProgress(1.0f)
{
}

StaminaBarWidget::~StaminaBarWidget()
{
}

void StaminaBarWidget::setProgress(float progress)
{
    CC_ASSERT(progress >= 0.0f && progress <= 1.0f);
    _logicalProgress += progress - _logicalProgress;
}

bool StaminaBarWidget::init()
{
    if (!cocos2d::Node::init()) {
        return false;
    }
    
    _bar = cocos2d::Sprite::create("ui/stamina_bar.png");
    _back = cocos2d::Sprite::create("ui/stamina_bar_back.png");
    _bar->setScaleX(_visibleProgress);
    
    addChild(_back, 0);
    addChild(_bar, 1);
    scheduleUpdate();
    setContentSize(_back->getContentSize());
    
    return true;
}

void StaminaBarWidget::update(float dt)
{
    float diff = _visibleProgress - _logicalProgress;
    float absdiff = fabsf(diff);
    if (absdiff > std::numeric_limits<float>::epsilon()) {
        float speed = 0.5f;
        float dir = diff > 0.0f ? -1.0f : 1.0f;
        if (speed * dt > absdiff) {
            _visibleProgress = _logicalProgress;
        } else {
            _visibleProgress += speed * dt * dir;
        }
        _bar->setScaleX(_visibleProgress);
    }
}
