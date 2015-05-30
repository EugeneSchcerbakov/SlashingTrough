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
    static EffectEnemyDeath* create(const std::string &enemySprite,
                                    const cocos2d::Vec2 posOnField,
                                    HeroWidget::SwordSide attackSide);
protected:
    EffectEnemyDeath();
    
    bool init(const std::string &enemySprite, const cocos2d::Vec2 posOnField, HeroWidget::SwordSide attackSide);
    
private:
    cocos2d::Sprite *_enemy;
    cocos2d::Sprite *_blood;
    cocos2d::ProgressTimer *_clip;
};

#endif /* defined(__SlashingTrough__EffectEnemyDeath__) */
