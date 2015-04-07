//
//  CharacterWidget.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 06/04/15.
//
//

#include "CharacterWidget.h"

CharacterWidget* CharacterWidget::create(Character::WeakPtr character)
{
    CharacterWidget *widget = new CharacterWidget(character);
    if (widget && widget->init()) {
        widget->autorelease();
    } else {
        delete widget;
        widget = nullptr;
    }
    return widget;
}

CharacterWidget::CharacterWidget(Character::WeakPtr character)
: _character(character)
{
}

CharacterWidget::~CharacterWidget()
{
}

bool CharacterWidget::init()
{
    if (!cocos2d::Node::create()) {
        return false;
    }
    
    _sword = cocos2d::DrawNode::create();
    _body = cocos2d::DrawNode::create();
    _bodyBorder = cocos2d::DrawNode::create();
    
    const cocos2d::Color4F bodyColor = cocos2d::Color4F(0.0f, 0.7f, 1.0f, 0.4f);
    const cocos2d::Color4F borderColor = cocos2d::Color4F(0.0f, 0.0f, 1.0f, 1.0f);
    const float radius = 80.0f;
    
    _bodyBorder->setPosition(cocos2d::Vec2(0.0f, 0.0f));
    _bodyBorder->drawCircle(cocos2d::Vec2(0.0f, 0.0f), radius, 0.0f, 25, false, 1.0f, 0.9f, borderColor);
    
    _body->setPosition(cocos2d::Vec2(0.0f, 0.0f));
    _body->drawSolidCircle(cocos2d::Vec2(0.0f, 0.0f), radius, 0.0f, 25, 1.0f, 0.9f, bodyColor);
    _body->addChild(_bodyBorder);
    
    _sword->drawSegment(cocos2d::Vec2(0.0f, 0.0f), cocos2d::Vec2(0.0f, 250.0f), 9.0f, cocos2d::Color4F::BLACK);
    _sword->setPosition(cocos2d::Vec2(60.0f, 0.0f));
    _sword->setRotation(160.0f);
    
    addChild(_sword, DrawOrder::SWORD);
    addChild(_body, DrawOrder::BODY);
    
    return true;
}


