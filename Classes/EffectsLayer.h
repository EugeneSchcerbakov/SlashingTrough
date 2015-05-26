//
//  EffectsLayer.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 26/05/15.
//
//

#ifndef __SlashingTrough__EffectsLayer__
#define __SlashingTrough__EffectsLayer__

#include "cocos2d.h"

class EffectsLayer;

class Effect : public cocos2d::Node
{
public:
    Effect(const std::string &name, int zOrder);
    
    virtual bool init();
    virtual void update(float dt);
    virtual bool isFinshed() const;
    
private:
    std::string _name;
    const int _zOrder;
    
    friend class EffectsLayer;
};

class EffectsLayer : public cocos2d::Layer
{
public:
    static EffectsLayer* create();
    
    void AddEffect(Effect *effect);
    
protected:
    EffectsLayer();
    
    bool init();
    void update(float dt);
    
private:
    typedef std::vector<Effect *> Effects;
    
private:
    Effects _effects;
};

#endif /* defined(__SlashingTrough__EffectsLayer__) */
