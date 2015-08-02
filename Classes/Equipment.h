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

#include "WeaponAbilities.h"

struct Equip
{
    typedef std::shared_ptr<Equip> Ptr;
    typedef std::weak_ptr<Equip> WeakPtr;
    
    enum class Type
    {
        NONE,
        WEAPON,
        ARMOR
    };
    
    Equip(Type t);
    virtual ~Equip();
    
    bool sold;
    int price;
    std::string id;
    std::string desc;
    std::string icon;
    std::string name;
    const Type type;
};

struct EquipWeapon : public Equip
{
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
    typedef std::vector<WeaponAbility::Ptr> Abilities;
    
    EquipWeapon();
    
    float damage;
    float speed;
    float distance;
    std::string sprite;
    TrailInfo trail;
    Abilities abilities;
};

struct EquipArmor : public Equip
{
    static Equip::Ptr create();
    static EquipArmor* cast(Equip::Ptr base);
    
    EquipArmor();
    
    float addHealth;
    std::string sprite;
};


#endif /* defined(__SlashingTrough__Equipment__) */
