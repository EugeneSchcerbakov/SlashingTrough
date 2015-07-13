//
//  ModelBase.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 11/07/15.
//
//

#include "ModelBase.h"

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

void ModelBase::sendEvent(const Event &event)
{
    _accepter(event, _parameter);
}

void ModelBase::setupAccepter(const Accepter &accepter, void *parameter)
{
    _accepter = accepter;
    _parameter = parameter;
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

void Entity::addHealth(float health)
{
    _health += health;
    if (_health < 0.0f) {
        _health = 0.0f;
    }
}

void Entity::onDamageReceived()
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
