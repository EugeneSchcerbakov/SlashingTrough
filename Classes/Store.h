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
    static Store& Instance();
    
    void LoadStore(const std::string &filename);
    void Buy(const std::string &id);
    
    Equip::Ptr GetItemById(const std::string &id) const;
    
    const Items& GetAllItems() const;
    Items GetWeaponItems() const;
    
private:
    void Reset();
    
    Items _items;
};

#endif /* defined(__SlashingTrough__Store__) */
