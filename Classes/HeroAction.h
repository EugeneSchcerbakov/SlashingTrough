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
        JUMP,
        ATTACK
    };
    
    typedef std::shared_ptr<HeroAction> Ptr;
    typedef std::weak_ptr<HeroAction> WeakPtr;
    
public:
    HeroAction(Hero *hero, float duration, Type type);
    virtual ~HeroAction();
    
    virtual void start(float currentY);
    virtual void update(float dt);
    virtual void setEvent(const Event &event);
    virtual bool isFinished() const;
    virtual bool isStarted() const;
    
    bool isType(Type type) const;
    bool isTag(const std::string &tag) const;
    
    const Event& getEvent() const;
    void setTag(const std::string &tag);
    
protected:
    Hero *_hero;
    Event _event;
    std::string _tag;
    
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
    static Ptr create(Hero *hero, float duration, float deltaX, float deltaY);
    
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
	static Ptr create(Hero *hero, float duration, float jumpDist, bool invulnerable);
    
	JumpBack(Hero *hero, float duration, float jumpDist, bool invulnerable);
    
    virtual void start(float y) override;
    virtual void update(float dt) override;
    
    float getFinishY() const;
    
private:
    float _jumpDist;
    float _startY;
    float _endY;
	const bool _invulnerable;
};

class JumpForwardAttack : public JumpBack
{
public:
	static Ptr create(Hero *hero, float duration, float jumpDist, bool invulnerable);
    
	JumpForwardAttack(Hero *hero, float duration, float jumpDist, bool invulnerable);
    
    virtual void update(float dt) override;
    
private:
    float _attackPeriodTime;
};

#endif /* defined(__SlashingTrough__HeroAction__) */
