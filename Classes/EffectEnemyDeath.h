//
//  EffectEnemyDeath.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 29/05/15.
//
//

#ifndef __SlashingTrough__EffectEnemyDeath__
#define __SlashingTrough__EffectEnemyDeath__

#include "EffectsLayer.h"
#include "HeroWidget.h"

class EffectEnemyDeath : public Effect
{
public:
    static EffectEnemyDeath* create(const std::string &sprite, const cocos2d::Vec2 pos);
    
protected:
    EffectEnemyDeath();
    
    bool init(const std::string &sprite, const cocos2d::Vec2 pos);
    
private:
    cocos2d::Sprite *_enemy;
    cocos2d::Sprite *_blood;
};

#endif /* defined(__SlashingTrough__EffectEnemyDeath__) */