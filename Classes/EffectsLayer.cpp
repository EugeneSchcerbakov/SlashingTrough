//
//  EffectsLayer.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 26/05/15.
//
//

#include "EffectsLayer.h"

// Effect implementation

Effect::Effect(const std::string &name, int zOrder)
: Sprite()
, _name(name)
, _zOrder(zOrder)
, _isFinished(false)
, _ancestor(nullptr)
{
}

Effect* Effect::create(const std::string &texture)
{
    Effect *effect = new Effect("Noname", 0);
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
    _isFinished = true;
}

bool Effect::isFinished() const
{
    return _isFinished;
}

// EffectsLayer implementation

const unsigned short EffectsLayer::TargetColor = (unsigned short)cocos2d::CameraFlag::USER1;
const unsigned short EffectsLayer::TargetDistor = (unsigned short)cocos2d::CameraFlag::USER2;

EffectsLayer* EffectsLayer::create()
{
    EffectsLayer *layer = new EffectsLayer();
    if (layer && layer->init()) {
        layer->autorelease();
    } else {
        delete layer;
        layer = nullptr;
    }
    return layer;
}

EffectsLayer::EffectsLayer()
{
}

void EffectsLayer::addEffect(Effect *effect)
{
    if (!effect) {
        CC_ASSERT(false);
        return;
    }
    
    for (Effect *eff : _effects) {
        if (eff == effect) {
            return;
        }
    }
    
    _effects.push_back(effect);
    addChild(effect, effect->_zOrder);
}

bool EffectsLayer::init()
{
    if (!cocos2d::Layer::init()) {
        return false;
    }
    scheduleUpdate();
    return true;
}

void EffectsLayer::update(float dt)
{
    for (Effects::iterator it = _effects.begin(); it != _effects.end();) {
        Effect *effect = (*it);
        if (effect->isFinished()) {
            removeChild(effect);
            it = _effects.erase(it);
        } else {
            ++it;
        }
    }
}
