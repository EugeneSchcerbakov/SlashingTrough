//
//  Item.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 06/06/15.
//
//

#ifndef __SlashingTrough__Item__
#define __SlashingTrough__Item__

#include <string>
#include <memory>
#include <vector>

#include "ItemFeature.h"

class Item
{
public:
    typedef std::shared_ptr<Item> Ptr;
    typedef std::weak_ptr<Item> WeakPtr;
    
    enum class Type
    {
        NONE,
        WEAPON,
        ARMOR
    };
    
public:
    Item(Type t);
    virtual ~Item();

    int getPrice() const;
    Type getType() const;
    
    const std::string& getId() const;
    const std::string& getDesc() const;
    const std::string& getIcon() const;
    const std::string& getName() const;

    bool isType(Type t) const;
    
protected:
    int price;
    std::string id;
    std::string desc;
    std::string icon;
    std::string name;
    const Type type;
    
    friend class Store;
};

class ItemWeapon : public Item
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
    
    static Item::Ptr create();
    static ItemWeapon* cast(Item::Ptr base);
    
public:
    ItemWeapon();
    
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

class ItemArmor : public Item
{
public:
    static Item::Ptr create();
    static ItemArmor* cast(Item::Ptr base);
    
public:
    ItemArmor();
    
    float getExtraHealth() const;
    const std::string& getSprite() const;
    
protected:
    float addHealth;
    std::string sprite;
    
    friend class Store;
};


#endif /* defined(__SlashingTrough__Item__) */
