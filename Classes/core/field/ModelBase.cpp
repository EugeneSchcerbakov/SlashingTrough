//
//  ModelBase.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 11/07/15.
//
//

#include "ModelBase.h"

#include "Utils.h"

Event::Event()
: _name("")
{
}

Event::Event(const std::string &name)
: _name(name)
{
}

bool Event::is(const std::string &name) const
{
    return _name == name;
}

ModelBase::ModelBase()
: _uid(generateUid())
{
}

ModelBase::~ModelBase()
{
}

void ModelBase::registerEventHandler(const std::string& name, Event::Handler callback)
{
    auto handler = _eventHandlers.find(name);
    if (handler == _eventHandlers.end())
    {
        auto data = std::pair<std::string, Event::Handler>(name, callback);
        _eventHandlers.insert(data);
    }
}

void ModelBase::sendEvent(const Event &event)
{
    auto handler = _eventHandlers.find(event._name);
    if (handler != _eventHandlers.end())
    {
        auto callback = (*handler).second;
        callback(event.variables);
    }
}

Uid ModelBase::getUid() const
{
    return _uid;
}

Uid ModelBase::generateUid()
{
    static Uid uid = 0;
    ++uid;
    return uid;
}

Entity::Entity(Entity::Type type)
: ModelBase()
, _type(type)
, _x(0.0f)
, _y(0.0f)
, _radius(0.0f)
, _health(0.0f)
, _maxHealth(0.0f)
, _damage(0.0f)
, _goal(nullptr)
{
}

Entity::~Entity()
{
}

void Entity::idleUpdate(float dt)
{
}

void Entity::addHealth(float health, bool callDamageReceived)
{
    _health += health;
    if (_health < 0.0f) {
        _health = 0.0f;
    }
    
    if (health < 0.0f && callDamageReceived)
    {
        onDamageReceived(health);
    }
}

void Entity::extendRadius(float increase)
{
    _radius = std::max(0.0f, _radius + increase);
}

void Entity::onDamageReceived(float damage)
{
}

void Entity::kill()
{
    _health = 0.0f;
}

void Entity::setPosition(float x, float y)
{
    _x = x;
    _y = y;
}

void Entity::setPositionX(float x)
{
    _x = x;
}

void Entity::setPositionY(float y)
{
    _y = y;
}

void Entity::setGoal(Entity *goal)
{
    _goal = goal;
}

const std::string& Entity::getSpriteFilename() const
{
    return _sprite;
}

float Entity::getPositionX() const
{
    return _x;
}

float Entity::getPositionY() const
{
    return _y;
}

float Entity::getDamage() const
{
    return _damage;
}

float Entity::getHealth() const
{
    return _health;
}

float Entity::getMaxHealth() const
{
    return _maxHealth;
}

bool Entity::isAlive() const
{
    return _health > 0.0f;
}

bool Entity::isType(Entity::Type type) const
{
    return _type == type;
}

Entity::Type Entity::getType() const
{
    return _type;
}

Reward::Reward(int killP, int coinP, int staminaP, int scoreP)
: _killPoints(killP)
, _coinPoints(coinP)
, _staminaPoints(staminaP)
, _scorePoints(scoreP)
{
}

Reward::~Reward()
{
}

int Reward::getKillPoints() const
{
    return _killPoints;
}

int Reward::getCoinPoints() const
{
    return _coinPoints;
}

int Reward::getStaminaPoints() const
{
    return _staminaPoints;
}

int Reward::getScorePoints() const
{
    return _scorePoints;
}
