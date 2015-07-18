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
class Hero;

class Enemy : public Entity, public Reward
{
public:
    enum class State
    {
        IDLE,
        MELLE_PREPEAR,
        MELLE_ATTACK_BEGIN,
        MELLE_ATTACK_END
    };
    
public:
    Enemy(const GameInfo::EnemyType &type, Field *field);
    
    void idleUpdate(float dt) override;
    void onDamageReceived() override;
    
    float getMelleAreaCenterX() const;
    float getMelleAreaCenterY() const;
    float getMelleAreaWidth() const;
    float getMelleAreaHeight() const;
    
    bool isState(State state) const;
    bool isMelleType(GameInfo::EnemyType::Melle type) const;
    
protected:
    virtual void processRangeAttack(Hero *hero, float dt, float len);
    virtual void processMelleAttack(Hero *hero, float dt, float len);
    
private:
    void init(const GameInfo::EnemyType &type);
    bool checkMelleZone(float px, float py) const;
    
    State _state;
    State _nextState;
    Field *_field;
    
    GameInfo::EnemyType::RangeAttack _rangeAttack;
    GameInfo::EnemyType::MelleAttack _melleAttack;
    
    float _rangeAttackTimer;
    float _melleAttackTimer;
    float _staminaDrainPoints;
    float _stateTimer;
    
    float _melleAreaCenterX;
    float _melleAreaCenterY;
    float _melleAreaWidth;
    float _melleAreaHeight;
    
    bool _melleAttacked;
    
    const float _melleDuration;
};

#endif /* defined(__SlashingTrough__Enemy__) */
