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

#include "Equipment.h"

class Store
{
public:
    typedef std::vector<Equip::Ptr> Items;
    
public:
    static Store& getInstance();
    
    void loadStore(const std::string &filename);
    void buy(const std::string &id);
    
    Equip::Ptr getItemById(const std::string &id) const;
    
    const Items& getAllItems() const;
    Items getWeaponItems() const;
    
private:
    void reset();
    
    Items _items;
};

#endif /* defined(__SlashingTrough__Store__) */
