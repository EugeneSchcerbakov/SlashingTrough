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

struct Equip
{
    typedef std::shared_ptr<Equip> Ptr;
    typedef std::weak_ptr<Equip> WeakPtr;
    
    enum class Type
    {
        NONE,
        WEAPON,
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
    static Equip::Ptr create();
    static EquipWeapon* cast(Equip::Ptr base);
    
    EquipWeapon();
    
    float damage;
    float speed;
    float distance;
    std::string sprite;
};


#endif /* defined(__SlashingTrough__Equipment__) */
