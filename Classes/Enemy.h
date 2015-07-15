//
//  Enemy.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 12/07/15.
//
//

#ifndef __SlashingTrough__Enemy__
#define __SlashingTrough__Enemy__

#include <stdio.h>

#include "ModelBase.h"
#include "GameInfo.h"

class Field;

class Enemy : public Entity, public Reward
{
public:
    Enemy(const GameInfo::EnemyType &type, Field *field);
    
    void idleUpdate(float dt) override;
    void onDamageReceived() override;
    
protected:
    virtual void processRangeAttack(float dt, float len);
    
private:
    void init(const GameInfo::EnemyType &type);
    
    Field *_field;
    
    float _rangeAttackTimer;
    float _staminaDrainPoints;
    
    GameInfo::EnemyType::RangeAttack _rangeAttack;
};

#endif /* defined(__SlashingTrough__Enemy__) */
