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

class Enemy : public Entity, public Reward
{
public:
    Enemy(const GameInfo::EnemyType &type);
    
    void idleUpdate(float dt) override;
    void onDamageReceived() override;
    
private:
    void init(const GameInfo::EnemyType &type);
    
    float _staminaDrainPoints;
};

#endif /* defined(__SlashingTrough__Enemy__) */
