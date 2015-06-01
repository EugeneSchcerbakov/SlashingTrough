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

class Store
{
public:
    struct Item
    {
        enum class Type
        {
            NONE,
            WEAPON,
        };
        
        bool sold;
        int price;
        std::string id;
        std::string desc;
        std::string icon;
        std::string name;
        const Type type;
        
        Item(Type t = Type::NONE)
        : id("")
        , sold(false)
        , type(t)
        {}
        
        virtual ~Item() {}
    };
    
    struct WeaponItem : public Item
    {
        float damage;
        float speed;
        
        WeaponItem()
        : Item(Type::WEAPON)
        {}
    };
    
    typedef std::vector<Item> Items;
    
public:
    static Store& Instance();
    
    void LoadStore(const std::string &filename);
    Item GetItemById(const std::string &id) const;
    
    const Items& GetAllItems() const;
    
private:
    void Reset();
    
    Items _items;
};

#endif /* defined(__SlashingTrough__Store__) */
