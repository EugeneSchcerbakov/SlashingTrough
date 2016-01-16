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

class Effect : public cocos2d::Sprite
{
public:
    Effect(const std::string &name, int zOrder);
    
    static Effect* create(const std::string &texture);
    
    virtual bool init();
    virtual void update(float dt) override;
    virtual bool isFinished() const;
    
    void setAncestor(cocos2d::Node *ancestor);
    void finish();
    
protected:
    std::string _name;
    const int _zOrder;
    bool _isFinished;
    
    cocos2d::Node *_ancestor;
    
    friend class EffectsLayer;
};

class EffectsLayer : public cocos2d::Layer
{
public:
    static EffectsLayer* create();
    
    void addEffect(Effect *effect);
    
public:
    static const unsigned short TargetColor;
    static const unsigned short TargetDistor;
    
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
