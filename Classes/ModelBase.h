//
//  ModelBase.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 11/07/15.
//
//

#ifndef __SlashingTrough__ModelBase__
#define __SlashingTrough__ModelBase__

#include <stdio.h>
#include <vector>
#include <list>

#include "VariablesSet.h"

class Entity;

typedef unsigned int Uid;
typedef std::list<Entity *> Entities;

class Event
{
public:
    Event();
    Event(const std::string &name);
    bool is(const std::string &name) const;
    
    VariablesSet variables;
    
private:
    std::string _name;
};

class ModelBase
{
public:
    typedef void(*Accepter)(const Event &, void *);
    
public:
    ModelBase();
    virtual ~ModelBase();
    
    void sendEvent(const Event &event);
    void setupAccepter(const Accepter &accepter, void *parameter);
    
    Uid getUid() const;
    
private:
    static Uid generateUid();
    const Uid _uid;
    Accepter _accepter;
    void *_parameter;
};

class Entity : public ModelBase
{
public:
    enum class Type
    {
        NONE,
        HERO,
        ENEMY,
        OBSTACLE,
        PROJECTILE
    };
    
public:
    Entity(Entity::Type type);
    virtual ~Entity();
    
    virtual void idleUpdate(float dt);
    virtual void addHealth(float health);
    virtual void onDamageReceived();
    virtual void kill();
    
    virtual void setPosition(float x, float y);
    virtual void setPositionX(float x);
    virtual void setPositionY(float y);
    virtual void setGoal(Entity *goal);
    
    virtual const std::string& getSpriteFilename() const;
    
    virtual float getPositionX() const;
    virtual float getPositionY() const;
    virtual float getDamage() const;
    virtual float getHealth() const;

    virtual bool isAlive() const;
    virtual bool isType(Entity::Type type) const;
    
    virtual Type getType() const;
    
protected:
    Entity *_goal;
    Type _type;
    std::string _sprite;
    
    float _x;
    float _y;
    float _radius;
    float _health;
    float _damage;
};

class Reward
{
public:
    Reward(int killP, int coinP, int staminaP, int scoreP);
    virtual ~Reward();
    
    virtual int getKillPoints() const;
    virtual int getCoinPoints() const;
    virtual int getStaminaPoints() const;
    virtual int getScorePoints() const;
    
private:
    int _killPoints;
    int _coinPoints;
    int _staminaPoints;
    int _scorePoints;
};

#endif /* defined(__SlashingTrough__ModelBase__) */
