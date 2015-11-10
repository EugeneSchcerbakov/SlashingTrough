//
//  Hero.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 11/07/15.
//
//

#ifndef __SlashingTrough__Hero__
#define __SlashingTrough__Hero__

#include <list>

#include "ModelBase.h"
#include "HeroAction.h"
#include "Item.h"
#include "PlayerInfo.h"

class Hero : public Entity
{
public:
    Hero();
    
    void init();
    void idleUpdate(float dt) override;
    void onDamageReceived(float damage) override;
    void addHealth(float health, bool callDamageReceived = true) override;
    void attack();
    void flushScore();
    void refreshGoals(Entities *entities);
    void onSwipeLeft();
    void onSwipeRight();
    void onSwipeBack();
    void onSwipeForward();
    
    void addAction(HeroAction::WeakPtr actionPtr);
    void addKillsPoint(int killsPoint);
    void addCoinsPoint(int coinsPoint);
    void addScorePoint(int scorePoint);
    
    void setRunningSpeed(float speed);
    void setRunning(bool running);
    void setSideBorders(float left, float right);
    
    float getSpeed() const;
    HeroAction* getLastAction() const;
    ItemWeapon* getWeapon() const;
    ItemArmor* getArmor() const;
    PlayerInfo::Score getScore() const;
    
    Entities* getGoals();
    
    bool isActionsQueueFull() const;
    bool isAbleToPerform(HeroAction::WeakPtr actionPtr);
    bool isActionInQueue(const std::string &tag) const;
    
private:
    typedef std::list<HeroAction::Ptr> ActionSequence;
    
private:    
    ActionSequence _actionSequence;
    PlayerInfo::Equipment _equip;
    PlayerInfo::Score _score;
    Entities *_goals;
    
    float _runningSpeed;
    float _attackArea;
    float _squarePassChecker;
    
    float _leftSideBorder;
    float _rightSideBorder;
    
    const float _squareSize;

    int _actionsSequenceMaxSize;
    
    bool _running;
};

#endif /* defined(__SlashingTrough__Hero__) */
