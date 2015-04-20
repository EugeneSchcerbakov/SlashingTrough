//
//  GameplayObjects.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 12/04/15.
//
//

#ifndef __SlashingTrough__GameplayObjects__
#define __SlashingTrough__GameplayObjects__

#include "cocos2d.h"
#include "GameInfo.h"

#include <memory>

class GameplayObject
{
public:
    enum class Type
    {
        NONE,
        OBSTACLE,
        ENEMY,
    };
    
    typedef std::shared_ptr<GameplayObject> Ptr;
    typedef std::weak_ptr<GameplayObject> WeakPtr;
    typedef unsigned int UID;
    
    static const UID InvalidUID;
    
public:
    GameplayObject(UID uid);
    virtual ~GameplayObject();
    
    virtual void AddHealth(float health);
    virtual void SetLogicalPos(float x, float y);
    
    virtual bool IsAlive() const;
    virtual bool IsType(Type type) const;
    
    virtual Type GetType() const;
    virtual const std::string& GetSpriteFilename() const;
    virtual float GetLogicalX() const;
    virtual float GetLogicalY() const;
    
    UID GetUID() const;
    
protected:
    float _logicalX;
    float _logicalY;
    float _health;
    std::string _sprite;
    Type _type;
    
    const UID _uid;
};

class Obstacle : public GameplayObject
{
public:
    static GameplayObject::Ptr Create(const GameInfo::ObstacleType &info, UID uid);
    static Obstacle* Cast(GameplayObject::Ptr ptr);
    
public:
    Obstacle(const GameInfo::ObstacleType &info, UID uid);
    virtual ~Obstacle();
    
    void AddHealth(float health) override;
    
    bool IsDestructible() const;

private:
    bool _destructible;
};

class Enemy : public GameplayObject
{
public:
    static GameplayObject::Ptr Create(const GameInfo::EnemyType &info, UID uid);
    static Enemy* Cast(GameplayObject::Ptr ptr);
    
public:
    Enemy(const GameInfo::EnemyType &info, UID uid);
    virtual ~Enemy();
    
    float GetDamage() const;
    
private:
    float _damage;
};

#endif /* defined(__SlashingTrough__GameplayObjects__) */
