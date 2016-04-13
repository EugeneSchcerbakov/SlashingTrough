//
//  EffectExplosion.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 07/02/16.
//
//

#ifndef EffectExplosion_hpp
#define EffectExplosion_hpp

#include "EffectsLayer.h"


class EffectExplosion : public Effect
{
public:
    static EffectExplosion* create(const cocos2d::Vec3 &pos);
    
protected:
    EffectExplosion();
    virtual ~EffectExplosion();
    
    bool init(const cocos2d::Vec3 &pos);
};

#endif /* EffectExplosion_h */
