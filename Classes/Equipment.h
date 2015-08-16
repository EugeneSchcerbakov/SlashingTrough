//
//  Equipment.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 06/06/15.
//
//

#ifndef __SlashingTrough__Equipment__
#define __SlashingTrough__Equipment__

#include <string>
#include <memory>
#include <vector>

#include "EquipFeature.h"
#include "WeaponFeature.h"

class Equip
{
public:
    typedef std::shared_ptr<Equip> Ptr;
    typedef std::weak_ptr<Equip> WeakPtr;
    
    enum class Type
    {
        NONE,
        WEAPON,
        ARMOR
    };
    
public:
    Equip(Type t);
    virtual ~Equip();
    
    virtual void featuresInit(Hero *owner);
    virtual void featuresUpdate(float dt);
    virtual void featuresOnSwipeRight();
    virtual void featuresOnSwipeLeft();
    virtual void featuresOnSwipeBack();
    
    void setSold(bool flag);

    int getPrice() const;
    Type getType() const;
    
    const std::string& getId() const;
    const std::string& getDesc() const;
    const std::string& getIcon() const;
    const std::string& getName() const;
    
    bool isSold() const;
    bool isType(Type t) const;
    
protected:
    bool sold;
    int price;
    std::string id;
    std::string desc;
    std::string icon;
    std::string name;
    const Type type;
    
    EquipFeature::Features features;
    
    friend class Store;
};

class EquipWeapon : public Equip
{
public:
    struct TrailInfo
    {
        float length;
        float width;
        float posYCoeff;
        float opacity;
        std::string texture;
    };
    
    static Equip::Ptr create();
    static EquipWeapon* cast(Equip::Ptr base);
    
public:
    EquipWeapon();
    
    virtual void featuresOnHit(Entity *goal);
    virtual void featuresOnKill(Entity *goal);
    
    float getDamage() const;
    float getSpeed() const;
    float getDistance() const;
    
    const std::string& getSprite() const;
    const TrailInfo& getTrailInfo() const;
    
protected:
    float damage;
    float speed;
    float distance;
    std::string sprite;
    TrailInfo trail;
    
    friend class Store;
};

class EquipArmor : public Equip
{
public:
    static Equip::Ptr create();
    static EquipArmor* cast(Equip::Ptr base);
    
public:
    EquipArmor();
    
    float getExtraHealth() const;
    const std::string& getSprite() const;
    
protected:
    float addHealth;
    std::string sprite;
    
    friend class Store;
};


#endif /* defined(__SlashingTrough__Equipment__) */
