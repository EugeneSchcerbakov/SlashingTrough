//
//  Hero.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 11/07/15.
//
//

#ifndef __SlashingTrough__Hero__
#define __SlashingTrough__Hero__

#include <stdio.h>
#include <queue>

#include "ModelBase.h"
#include "HeroAction.h"
#include "Equipment.h"
#include "PlayerInfo.h"

class Hero : public Entity
{
public:
    Hero();
    
    void init();
    void idleUpdate(float dt) override;
    void onDamageReceived() override;
    void attack();
    void flushScore();
    void refreshGoals(Entities *entities);
    
    void addAction(HeroAction *action);
    void addStamina(float stamina);
    void addKillsPoint(int killsPoint);
    void addCoinsPoint(int coinsPoint);
    void addScorePoint(int scorePoint);
    
    void setWeapon(Equip::WeakPtr weapon);
    void setArmor(Equip::WeakPtr armor);
    void setRunningSpeed(float speed);
    void setRunning(bool running);
    void setSideBorders(float left, float right);
    
    float getSpeed() const;
    float getStamina() const;
    HeroAction* getLastAction() const;
    EquipWeapon* getWeapon() const;
    EquipArmor* getArmor() const;
    PlayerInfo::Score getScore() const;
    
    bool isActionsQueueFull() const;
    bool isAbleToPerform(HeroAction *action);
    
private:
    typedef std::queue<HeroAction *> ActionSequence;
    
private:    
    ActionSequence _actionSequence;
    Equip::WeakPtr _weapon;
    Equip::WeakPtr _armor;
    PlayerInfo::Score _score;
    Entities *_goals;
    
    float _runningSpeed;
    float _attackArea;
    float _damageUpValue;
    float _staminaPoints;
    float _staminaDrainTime;
    float _staminaDrainValue;
    float _staminaDrainTimeCounter;
    
    float _leftSideBorder;
    float _rightSideBorder;
    
    int _killPointToNextDamageUp;
    int _damageUpKillPoints;
    int _actionsSequenceMaxSize;
    
    bool _running;
};

#endif /* defined(__SlashingTrough__Hero__) */
