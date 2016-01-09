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

#include "Enemy.h"

class HealthBar;
class MelleHitZone;

class EnemyWidget : public cocos2d::BillBoard
{
public:
    static EnemyWidget* create(Enemy *enemy, cocos2d::Layer *fieldLayer);
    
    bool isDeletionAllowed() const;
    
protected:
	EnemyWidget(Enemy *enemy, cocos2d::Layer *fieldLayer);
    virtual ~EnemyWidget();
    
    bool init() override;
    void update(float dt) override;
    
    void runHitAccentEffect();
    
    void acceptEvent(const Event &event);
    
private:
    static void accepter(const Event &event, void *param);
    
    Enemy *_enemy;
    
	cocos2d::Layer *_fieldLayer;
    cocos2d::Node *_sprite;
    cocos2d::Sprite *_blood;
    cocos2d::Sprite *_weapon;
    HealthBar *_healhBar;
    MelleHitZone *_hitZoneWidget;
    
    float _scale;
    
    bool _allowDeletion;
};

#endif /* defined(__SlashingTrough__EnemyWidget__) */
