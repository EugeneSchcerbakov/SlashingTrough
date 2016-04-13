//
//  EffectFlyingIcon.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 18/10/15.
//
//

#include "EffectFlyingIcon.h"

#include "Utils.h"

EffectFlyingIcon* EffectFlyingIcon::create(std::string icon, cocos2d::Vec2 from,
                                                     cocos2d::Vec2 to, float startScale)
{
    EffectFlyingIcon *effect = new EffectFlyingIcon();
    if (effect && effect->init(icon, from, to, startScale)) {
        effect->autorelease();
    } else {
        delete effect;
        effect = nullptr;
    }
    return effect;
}

EffectFlyingIcon::EffectFlyingIcon()
: Effect()
{
}

EffectFlyingIcon::~EffectFlyingIcon()
{
}

void EffectFlyingIcon::setOpacity(GLubyte opacity)
{
    _sprite->setOpacity(opacity);
}

bool EffectFlyingIcon::init(std::string icon, cocos2d::Vec2 from, cocos2d::Vec2 to, float startScale)
{
    if (!Effect::init()) {
        return false;
    }
    
    _sprite = cocos2d::Sprite::create(icon);
    
    auto func = [this]()
    {
        finish();
    };
    
    cocos2d::Vec2 m, v0, v1;
    float angle = math::PI * 0.5f;
    
    m = (from - to);
    m = m.rotateByAngle(cocos2d::Vec2::ZERO, angle);
    m = m.getNormalized();
    v0 = (from * 0.67f + to * 0.33f) + m * 140.0f;
    v1 = (from * 0.33f + to * 0.67f) + m * 120.0f;
 
    cocos2d::PointArray *way = cocos2d::PointArray::create(4);
    way->addControlPoint(from);
    way->addControlPoint(v0);
    way->addControlPoint(v1);
    way->addControlPoint(to);
    
    float duration = 1.5f;
    
    auto scale0 = cocos2d::ScaleTo::create(duration * 0.3f, 2.5f);
    auto scale1 = cocos2d::ScaleTo::create(duration * 0.7f, 0.7f);
    auto scale = cocos2d::Sequence::create(scale0, scale1, nullptr);
    
    auto fade = cocos2d::FadeOut::create(duration * 0.4f);
    auto wait = cocos2d::DelayTime::create(duration * 0.6f);
    auto alpha = cocos2d::Sequence::create(wait, fade, nullptr);
    
    auto move = cocos2d::CardinalSplineTo::create(duration, way, 0.01f);
    auto ease = cocos2d::EaseSineInOut::create(move);
    auto call = cocos2d::CallFunc::create(func);
    
    auto motion = cocos2d::Spawn::create(ease, scale, alpha, nullptr);
    auto effect = cocos2d::Sequence::create(motion, call, nullptr);
    
    runAction(effect);
    
    setPosition(from);
    setScale(startScale);
    addChild(_sprite);
    
    return true;
}
