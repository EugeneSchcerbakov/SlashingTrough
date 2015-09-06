//
//  ItemFeature.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 15/08/15.
//
//

#ifndef __SlashingTrough__ItemFeature__
#define __SlashingTrough__ItemFeature__

#include <memory>
#include <vector>

class Hero;
class Entity;

class ItemFeature
{
public:
    typedef std::shared_ptr<ItemFeature> Ptr;
    typedef std::weak_ptr<ItemFeature> WeakPtr;
    typedef std::vector<ItemFeature::Ptr> Features;
    
public:
    ItemFeature();
    virtual ~ItemFeature();
    
public:
    virtual void init(Hero *owner);
    virtual void update(float dt);
    
    virtual void onSwipeBack();
    virtual void onSwipeRight();
    virtual void onSwipeLeft();
    
protected:
    Hero *_owner;
};

class Backsliding : public ItemFeature
{
public:
    static Ptr create(float cooldown, float distance, float duration);
    
public:
    Backsliding(float cooldown, float distance, float duration);
    
    void init(Hero *owner);
    void update(float dt) override;
    void onSwipeBack() override;
    
protected:
    static const std::string TAG;
    
    float _cooldown;
    float _distance;
    float _duration;
    float _time;
    bool _allow;
};

class BackslidingShield : public Backsliding
{
public:
    static Ptr create(float cooldown, float distance, float duration, float shieldTime);
    
public:
    BackslidingShield(float cooldown, float distance, float duration, float shieldTime);
    
    void update(float dt) override;
    void onSwipeBack() override;
    
protected:
    void updateBarrier();
    bool isPointUnderBarrier(float x, float y) const;
    
    float _shieldTimer;
    bool _shieldShown;
    
    const float _shieldLiveTime;
};

class WeaponFeature : public ItemFeature
{
public:
    WeaponFeature();
    
    virtual void onHit(Entity *goal);
    virtual void onKill(Entity *goal);
};

class CoinsForMurder : public WeaponFeature
{
public:
    static Ptr create(int flat, int percentOfEnemyCost);
    
public:
    CoinsForMurder(int flag, int percentOfEnemyCost);
    
    void onKill(Entity *goal) override;
    
private:
    const int _flat;
    const int _percentOfEnemyCost;
};

#endif /* defined(__SlashingTrough__ItemFeature__) */
