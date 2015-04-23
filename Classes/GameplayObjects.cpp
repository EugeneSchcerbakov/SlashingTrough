//
//  GameplayObjects.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 12/04/15.
//
//

#include "GameplayObjects.h"

const GameplayObject::UID GameplayObject::InvalidUID = 0;

GameplayObject::GameplayObject(Type type, UID uid)
: _type(type)
, _uid(uid)
{
}

GameplayObject::~GameplayObject()
{
}

bool GameplayObject::Attack(GameplayObject::Ptr object, float distance)
{
    if (object->IsAlive() && distance <= _radius) {
        object->AddHealth(-_damage);
        return true;
    }
    return false;
}

void GameplayObject::Kill()
{
    _health = 0.0f;
}

void GameplayObject::AddHealth(float health)
{
    _health += health;
}

void GameplayObject::SetLogicalPos(float x, float y)
{
    _logicalX = x;
    _logicalY = y;
}

bool GameplayObject::IsAlive() const
{
    return _health > 0.0f;
}

bool GameplayObject::IsType(Type type) const
{
    return _type == type;
}

GameplayObject::Type GameplayObject::GetType() const
{
    return _type;
}

const std::string& GameplayObject::GetSpriteFilename() const
{
    return _sprite;
}

float GameplayObject::GetHealth() const
{
    return _health;
}

float GameplayObject::GetLogicalX() const
{
    return _logicalX;
}

float GameplayObject::GetLogicalY() const
{
    return _logicalY;
}

GameplayObject::UID GameplayObject::GetUID() const
{
    return _uid;
}

// Obstacle implementation

GameplayObject::Ptr Obstacle::Create(const GameInfo::ObstacleType &info, UID uid)
{
    return std::make_shared<Obstacle>(info, uid);
}

Obstacle* Obstacle::Cast(GameplayObject::Ptr ptr)
{
    return static_cast<Obstacle *>(ptr.get());
}

Obstacle::Obstacle(const GameInfo::ObstacleType &info, UID uid)
: GameplayObject(Type::OBSTACLE, uid)
{
    _health = info.health;
    _sprite = info.sprite;
    _damage = info.damage;
    _radius = 150.0f;
    _destructible = info.destructible;
}

Obstacle::~Obstacle()
{
}

void Obstacle::AddHealth(float health)
{
    if (health < 0.0f && !_destructible) {
        return;
    } else {
        GameplayObject::AddHealth(health);
    }
}

bool Obstacle::IsDestructible() const
{
    return _destructible;
}

// Enemy implementation

GameplayObject::Ptr Enemy::Create(const GameInfo::EnemyType &info, UID uid)
{
    return std::make_shared<Enemy>(info, uid);
}

Enemy* Enemy::Cast(GameplayObject::Ptr ptr)
{
    return static_cast<Enemy *>(ptr.get());
}

Enemy::Enemy(const GameInfo::EnemyType &info, UID uid)
: GameplayObject(Type::ENEMY, uid)
{
    _health = info.health;
    _sprite = info.sprite;
    _damage = info.damage;
    _radius = 150.0f;
}

Enemy::~Enemy()
{
}

