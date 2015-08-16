//
//  Store.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 01/06/15.
//
//

#ifndef __SlashingTrough__Store__
#define __SlashingTrough__Store__

#include <string>
#include <vector>
#include <memory>
#include "tinyxml2/tinyxml2.h"

#include "Equipment.h"

class Store
{
public:
    typedef std::vector<Equip::Ptr> Items;
    
    static const std::string DEFAULT_WEAPON_ID;
    static const std::string DEFAULT_ARMOR_ID;
    
public:
    static Store& getInstance();
    
    void loadStore(const std::string &filename);
    bool buy(const std::string &id);
    
    Equip::Ptr getItemById(const std::string &id) const;
    
    const Items& getAllItems() const;
    Items getWeaponItems() const;
    Items getArmorItems() const;
    
private:
    void parseEquipFeature(Equip::WeakPtr equip, tinyxml2::XMLElement *root);
    
    void reset();
    
    Items _items;
};

#endif /* defined(__SlashingTrough__Store__) */
