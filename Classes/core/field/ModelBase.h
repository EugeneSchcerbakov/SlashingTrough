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
#include <memory>
#include <functional>
#include <string>
#include <map>

#include "VariablesSet.h"

class Entity;

typedef unsigned int Uid;
typedef std::list<Entity *> Entities;

#define BIND_EVENT_HANDLER(__handler__, __target__) std::bind(&__handler__, __target__, std::placeholders::_1)

class Event
{
    friend class ModelBase;

public:
    typedef std::function<void(const VariablesSet&)> Handler;

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
    ModelBase();
    virtual ~ModelBase();

    void registerEventHandler(const std::string& name, Event::Handler callback);
    void sendEvent(const Event &event);

    Uid getUid() const;
    
private:
    static Uid generateUid();
    const Uid _uid;

    std::map<std::string, Event::Handler> _eventHandlers;
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
    virtual void addHealth(float health, bool callDamageReceived = true);
    virtual void extendRadius(float increase);
    virtual void onDamageReceived(float damage);
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
    virtual float getMaxHealth() const;

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
    float _maxHealth;
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
