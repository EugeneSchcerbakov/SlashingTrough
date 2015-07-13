//
//  HeroAction.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 11/07/15.
//
//

#ifndef __SlashingTrough__HeroAction__
#define __SlashingTrough__HeroAction__

#include "ModelBase.h"

class Hero;

class HeroAction
{
public:
    enum class Type
    {
        NONE,
        ATTACK_MOVE,
        JUMPBACK,
        ATTACK
    };
    
public:
    HeroAction(Hero *hero, float duration, Type type);
    virtual ~HeroAction();
    
    virtual void start();
    virtual void update(float dt);
    virtual void setEvent(const Event &event);
    virtual bool isFinished() const;
    virtual bool isStarted() const;
    
    bool isType(Type type) const;
    const Event& getEvent() const;
    
protected:
    Hero *_hero;
    Event _event;
    
    float _duration;
    float _localTime;
    
    bool _isStarted;
    bool _isAttacked;
    bool _isFinished;
    
    const Type _type;
};

class AttackAndMove : public HeroAction
{
public:
    AttackAndMove(Hero *hero, float duration, float deltaX, float deltaY);
    
    void update(float dt) override;
    
    float getFinishX() const;
    float getFinishY() const;
    
private:
    float _startX;
    float _startY;
    float _finishX;
    float _finishY;
};

class JumpBack : public HeroAction
{
public:
    JumpBack(Hero *hero, float duration, float jumpDist);
    
    void start() override;
    void update(float dt) override;
    
    float getFinishY() const;
    
private:
    float _startY;
    float _endY;
};

#endif /* defined(__SlashingTrough__HeroAction__) */
