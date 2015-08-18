//
//  EquipFeature.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 15/08/15.
//
//

#ifndef __SlashingTrough__EquipFeature__
#define __SlashingTrough__EquipFeature__

#include <memory>
#include <vector>

class Hero;
class Entity;

class EquipFeature
{
public:
    typedef std::shared_ptr<EquipFeature> Ptr;
    typedef std::weak_ptr<EquipFeature> WeakPtr;
    typedef std::vector<EquipFeature::Ptr> Features;
    
public:
    EquipFeature();
    virtual ~EquipFeature();
    
public:
    virtual void init(Hero *owner);
    virtual void update(float dt);
    
    virtual void onSwipeBack();
    virtual void onSwipeRight();
    virtual void onSwipeLeft();
    
protected:
    Hero *_owner;
};

class Backsliding : public EquipFeature
{
public:
    static Ptr create(float cooldown, float distance, float duration);
    
public:
    Backsliding(float cooldown, float distance, float duration);
    
    void init(Hero *owner);
    void update(float dt) override;
    void onSwipeBack() override;
    
private:
    static const std::string TAG;
    
    float _cooldown;
    float _distance;
    float _duration;
    float _time;
    bool _allow;
};

#endif /* defined(__SlashingTrough__EquipFeature__) */
