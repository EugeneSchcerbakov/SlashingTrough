//
//  EffectEnemyDeath.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 29/05/15.
//
//

#include "EffectEnemyDeath.h"

EffectEnemyDeath* EffectEnemyDeath::create(const std::string &enemySprite,
                                           const cocos2d::Vec2 posOnField,
                                           HeroWidget::SwordSide attackSide)
{
    EffectEnemyDeath *effect = new EffectEnemyDeath();
    if (effect && effect->init(enemySprite, posOnField, attackSide)) {
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

bool EffectEnemyDeath::init(const std::string &enemySprite,
                            const cocos2d::Vec2 posOnField,
                            HeroWidget::SwordSide attackSide)
{
    if (!Effect::init()) {
        return false;
    }
    
    _enemy = cocos2d::Sprite::create(enemySprite);
    _blood = cocos2d::Sprite::create("effects/blood_sprite.png");
    _blood->setScale(2.0);
    
    _clip = cocos2d::ProgressTimer::create(_blood);
    _clip->setScale(2.0f);
    _clip->setType(cocos2d::ProgressTimer::Type::BAR);
    _clip->setBarChangeRate(cocos2d::Vec2(1.0f, 0.0f));
    _clip->setPercentage(0.0f);
    _clip->setOpacity(0);
    // determin sprite clipping direction
    if (attackSide == HeroWidget::SwordSide::RIGHT) {
        _clip->setMidpoint(cocos2d::Vec2(1.0f, 0.5f));
    } else if (attackSide == HeroWidget::SwordSide::LEFT) {
        _clip->setMidpoint(cocos2d::Vec2(0.0f, 0.5f));
    }
    
    cocos2d::ProgressFromTo *clip = cocos2d::ProgressFromTo::create(0.2f, 0.0f, 100.0f);
    cocos2d::FadeIn *fadein = cocos2d::FadeIn::create(0.2f);
    cocos2d::Spawn *bloodShow = cocos2d::Spawn::create(clip, fadein, nullptr);
    cocos2d::FadeOut *fadeout = cocos2d::FadeOut::create(0.29f);
    cocos2d::Sequence *bloodEffect = cocos2d::Sequence::create(bloodShow, fadeout, nullptr);
    _clip->runAction(bloodEffect);
    
    auto effectFinish = [&]{_isFinished = true;};
    cocos2d::DelayTime *bodyDelay = cocos2d::DelayTime::create(0.2f); // wait for blood sprite
    cocos2d::FadeOut *bodyFadeout = cocos2d::FadeOut::create(0.3f);
    cocos2d::CallFunc *finishFunc = cocos2d::CallFunc::create(effectFinish);
    cocos2d::Sequence *bodyEffect = cocos2d::Sequence::create(bodyDelay, bodyFadeout, finishFunc, nullptr);
    _enemy->runAction(bodyEffect);
    
    setPosition(posOnField);
    addChild(_enemy, 0);
    addChild(_clip, 1);
    
    return true;
}

