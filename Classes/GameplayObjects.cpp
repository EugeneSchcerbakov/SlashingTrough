//
//  GameplayObjects.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 12/04/15.
//
//

#include "GameplayObjects.h"

const GameplayObject::UID GameplayObject::InvalidUID = 0;

GameplayObject::GameplayObject(UID uid)
: _type(Type::NONE)
, _uid(uid)
{
}

GameplayObject::~GameplayObject()
{
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

Obstacle::Obstacle(const GameInfo::ObstacleType &info, UID uid) : GameplayObject(uid)
{
    _type = Type::OBSTACLE;
    _health = info.health;
    _sprite = info.sprite;
    _destructible = info.destructible;
}

Obstacle::~Obstacle()
{
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

Enemy::Enemy(const GameInfo::EnemyType &info, UID uid) : GameplayObject(uid)
{
    _type = Type::ENEMY;
    _health = info.health;
    _sprite = info.sprite;
    _damage = info.damage;
}

Enemy::~Enemy()
{
}

float Enemy::GetDamage() const
{
    return _damage;
}
