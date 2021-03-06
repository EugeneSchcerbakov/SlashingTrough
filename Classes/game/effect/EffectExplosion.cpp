//
//  EffectExplosion.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 07/02/16.
//
//

#include "EffectExplosion.h"

#include "cocos-ext.h"
#include "audio/include/SimpleAudioEngine.h"
#include "MagicEmitter.h"
#include "Utils.h"
#include <array>

EffectExplosion* EffectExplosion::create(const cocos2d::Vec3 &pos)
{
    EffectExplosion *effect = new EffectExplosion();
    if (effect && effect->init(pos)) {
        effect->autorelease();
    } else {
        delete effect;
        effect = nullptr;
    }
    return effect;
}

EffectExplosion::EffectExplosion()
: Effect()
{
}

EffectExplosion::~EffectExplosion()
{
}

bool EffectExplosion::init(const cocos2d::Vec3 &pos)
{
    if (!Effect::init()) {
        return false;
    }

    std::array<std::string, 3> effectsId = {
            "Fire explosion 2",
            "Fire explosion 3",
            "Fire explosion 4"
    };

    int index = misc::random(0, effectsId.size() - 1);
    std::string effectId = effectsId[index];

    MagicEmitter *hit = MagicEmitter::create(effectId.c_str());
    hit->setCameraMask(Effect::TargetColor);
    hit->setPosition3D(pos);
    hit->setScale(1.25f);
    addChild(hit);
    
    auto *distortion = cocos2d::Sprite::create("effects/circle_distortion.png");
    distortion->setPosition3D(pos);
    distortion->setCameraMask(Effect::TargetDistor);
    distortion->setScale(0.3f);
    distortion->setOpacity(0);
    addChild(distortion);
    
    auto finish = [this](){Effect::finish();};
    auto callback = cocos2d::CallFunc::create(finish);
    auto fadein = cocos2d::FadeIn::create(0.1f);
    auto fadeout = cocos2d::FadeOut::create(0.3f);
    auto fadestay = cocos2d::DelayTime::create(0.2f);
    auto fade = cocos2d::Sequence::create(fadein, fadestay, fadeout, nullptr);
    auto scale = cocos2d::ScaleTo::create(1.0f, 2.0f);
    auto effect = cocos2d::Spawn::create(fade, scale, nullptr);
    auto delay = cocos2d::DelayTime::create(0.2f);
    auto seq = cocos2d::Sequence::create(delay, effect, callback, nullptr);
    
    distortion->runAction(seq);

    auto audioEngine = CocosDenshion::SimpleAudioEngine::getInstance();
    audioEngine->playEffect("fire_05.mp3");
    
    return true;
}
