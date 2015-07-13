//
//  EffectEnemyDeath.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 29/05/15.
//
//

#include "EffectEnemyDeath.h"

EffectEnemyDeath* EffectEnemyDeath::create(const std::string &sprite, const cocos2d::Vec2 pos)
{
    EffectEnemyDeath *effect = new EffectEnemyDeath();
    if (effect && effect->init(sprite, pos)) {
        effect->autorelease();
    } else {
        delete effect;
        effect = nullptr;
    }
    return effect;
}

EffectEnemyDeath::EffectEnemyDeath()
: Effect("EnemyDeapth", 1)
{
}

bool EffectEnemyDeath::init(const std::string &sprite, const cocos2d::Vec2 pos)
{
    if (!Effect::init()) {
        return false;
    }
    
    _enemy = cocos2d::Sprite::create(sprite);
    _blood = cocos2d::Sprite::create("effects/blood_sprite.png");
    _blood->setScale(2.0);
    _blood->setOpacity(0);
    
    cocos2d::FadeIn *fadein = cocos2d::FadeIn::create(0.2f);
    cocos2d::FadeOut *fadeout = cocos2d::FadeOut::create(0.29f);
    cocos2d::Sequence *bloodEffect = cocos2d::Sequence::create(fadein, fadeout, nullptr);
    _blood->runAction(bloodEffect);
    
    auto effectFinish = [&]{_isFinished = true;};
    cocos2d::DelayTime *bodyDelay = cocos2d::DelayTime::create(0.2f); // wait for blood sprite
    cocos2d::FadeOut *bodyFadeout = cocos2d::FadeOut::create(0.3f);
    cocos2d::CallFunc *finishFunc = cocos2d::CallFunc::create(effectFinish);
    cocos2d::Sequence *bodyEffect = cocos2d::Sequence::create(bodyDelay, bodyFadeout, finishFunc, nullptr);
    _enemy->runAction(bodyEffect);
    
    setPosition(pos);
    addChild(_enemy, 0);
    addChild(_blood, 1);
    
    return true;
}