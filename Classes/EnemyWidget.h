//
//  EnemyWidget.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 12/07/15.
//
//

#ifndef __SlashingTrough__EnemyWidget__
#define __SlashingTrough__EnemyWidget__

#include "Enemy.h"

#include "cocos2d.h"

class HealthBar;
class MelleHitZone;

class EnemyWidget : public cocos2d::BillBoard
{
public:
    static EnemyWidget* create(Enemy *enemy, cocos2d::Layer *fieldEffects);
    
    bool isDeletionAllowed() const;
    
protected:
	EnemyWidget(Enemy *enemy, cocos2d::Layer *fieldEffects);
    virtual ~EnemyWidget();
    
    bool init() override;
    void update(float dt) override;
    
    void runHitAccentEffect();
    
private:
    void _handleEventDamageReceived(const VariablesSet& args);
	void _handleEventFatalDamageReceived(const VariablesSet& args);
	void _handleEventShowMelleHighlight(const VariablesSet& args);
	void _handleEventShowMelleAttack(const VariablesSet& args);
	void _handleEventHitedByProjectile(const VariablesSet& args);
	void _handleEventProjectileShot(const VariablesSet& args);

    Enemy *_enemy;
    
	cocos2d::Layer*_fieldEffects;
    cocos2d::Node *_sprite;
    cocos2d::Sprite *_blood;
    cocos2d::Sprite *_weapon;
    HealthBar *_healhBar;
    MelleHitZone *_hitZoneWidget;
    
    float _scale;
    
    bool _allowDeletion;
};

#endif /* defined(__SlashingTrough__EnemyWidget__) */
