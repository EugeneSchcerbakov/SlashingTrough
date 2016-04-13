//
//  EffectsLayer.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 26/05/15.
//
//

#include "EffectsLayer.h"

// Effect implementation

const unsigned short Effect::TargetColor = (unsigned short)cocos2d::CameraFlag::USER1;
const unsigned short Effect::TargetDistor = (unsigned short)cocos2d::CameraFlag::USER2;

Effect::Effect()
: Sprite()
, _ancestor(nullptr)
{
}

Effect* Effect::create(const std::string &texture)
{
    Effect *effect = new Effect();
    if (effect && effect->initWithFile(texture)) {
        effect->autorelease();
    } else {
        delete effect;
        effect = nullptr;
    }
    return effect;
}

bool Effect::init()
{
    if (!cocos2d::Sprite::init()) {
        return false;
    }
    
    return true;
}

void Effect::update(float dt)
{
    if (_ancestor)
    {
        setPosition3D(_ancestor->getPosition3D());
    }
}

void Effect::setAncestor(cocos2d::Node *ancestor)
{
    _ancestor = ancestor;
}

void Effect::finish()
{
    removeFromParentAndCleanup(true);
}
