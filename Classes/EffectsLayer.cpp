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
: _name(name)
, _zOrder(zOrder)
, _isFinished(false)
{
}

bool Effect::init()
{
    if (!cocos2d::Node::init()) {
        return false;
    }
    return true;
}

bool Effect::isFinished() const
{
    return _isFinished;
}

// EffectsLayer implementation

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
