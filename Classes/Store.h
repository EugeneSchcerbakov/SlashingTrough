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

#include "Item.h"

class Store
{
public:
    typedef std::vector<Item::Ptr> Items;
    
public:
    static Store& getInstance();
    
    void loadStore(const std::string &filename);
    bool buy(const std::string &id);
    
    Item::Ptr getItemById(const std::string &id) const;
    
    const Items& getAllItems() const;
    Items getItemsWithType(Item::Type type) const;
    
private:    
    void reset();
    
    Items _items;
};

#endif /* defined(__SlashingTrough__Store__) */
