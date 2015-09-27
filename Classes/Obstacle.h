//
//  Obstacle.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 12/07/15.
//
//

#ifndef __SlashingTrough__Obstacle__
#define __SlashingTrough__Obstacle__

#include <stdio.h>

#include "ModelBase.h"
#include "GameInfo.h"

class Obstacle : public Entity, public Reward
{
public:
    Obstacle(const GameInfo::ObstacleType &type);
    
    void idleUpdate(float dt) override;
    void addHealth(float health, bool callDamageReceived = true) override;
    
private:
    void init(const GameInfo::ObstacleType &type);
    
    float _staminaDrainPoints;
    bool _destructible;
};

#endif /* defined(__SlashingTrough__Obstacle__) */
