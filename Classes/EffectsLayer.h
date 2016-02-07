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
    static const unsigned short TargetColor;
    static const unsigned short TargetDistor;
    
public:
    Effect();
    
    static Effect* create(const std::string &texture);
    
    virtual bool init() override;
    virtual void update(float dt) override;
    
    void setAncestor(cocos2d::Node *ancestor);
    void finish();
    
protected:    
    cocos2d::Node *_ancestor;
};

#endif /* defined(__SlashingTrough__EffectsLayer__) */
