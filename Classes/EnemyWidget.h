//
//  EnemyWidget.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 12/07/15.
//
//

#ifndef __SlashingTrough__EnemyWidget__
#define __SlashingTrough__EnemyWidget__

#include "cocos2d.h"

#include "EffectsLayer.h"
#include "Enemy.h"

class HealthBar;

class EnemyWidget : public cocos2d::Node
{
public:
    static EnemyWidget* create(Enemy *enemy, EffectsLayer *effects);
    
protected:
    EnemyWidget(Enemy *enemy, EffectsLayer *effects);
    virtual ~EnemyWidget();
    
    bool init() override;
    void update(float dt) override;
    
    void runHitAccentEffect();
    
    void acceptEvent(const Event &event);
    
private:
    static void accepter(const Event &event, void *param);
    
    Enemy *_enemy;
    EffectsLayer *_effects;
    
    cocos2d::Sprite *_sprite;
    cocos2d::Sprite *_blood;
    HealthBar *_healhBar;
};

#endif /* defined(__SlashingTrough__EnemyWidget__) */
