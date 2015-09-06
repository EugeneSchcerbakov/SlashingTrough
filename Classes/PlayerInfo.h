//
//  PlayerInfo.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 07/06/15.
//
//

#ifndef __SlashingTrough__PlayerInfo__
#define __SlashingTrough__PlayerInfo__

#include <vector>
#include <string>

#include "Item.h"
#include "VariablesSet.h"

class PlayerInventory
{
public:
    struct PlayerItem
    {
        Item::Ptr item;
        int amount;
        
        PlayerItem() = default;
    };
    
public:
    void add(Item::WeakPtr item, int amount = 1);
    void remove(Item::WeakPtr item);
    
    int getAmount(const std::string &id) const;
    int getAmount(Item::WeakPtr item) const;
    
    bool owned(const std::string &id) const;
    bool owned(Item::WeakPtr item) const;
    
    PlayerItem& operator[](const std::string &id);
    
    friend class PlayerInfo;
    
private:
    std::map<std::string, PlayerItem> _items;
};

class PlayerInfo
{
public:
    static const std::string VarKeyCoins;
    static const std::string VarKeyItemWpn;
    static const std::string VarKeyItemArm;
    
    static const std::string DEFAULT_WEAPON_ID;
    static const std::string DEFAULT_ARMOR_ID;
    
public:
    struct Score
    {
        int coins;
        int kills;
        int score;
        Score()
        : coins(0)
        , kills(0)
        , score(0)
        {}
    };
    
public:
    static PlayerInfo& getInstance();
    
    PlayerInfo();
    
    void load(const std::string &filename);
    void save();
    
    void addCoins(int coins);
    void equip(Item::Ptr item);
    bool equipped(Item::Ptr item) const;

    int getCoins() const;
    int getDamage() const;
    
    VariablesSet variables;
    PlayerInventory Inventory;
    
private:
    std::vector<std::string> _ownedEquips;
    std::string _saveFileName;
};

#endif /* defined(__SlashingTrough__PlayerInfo__) */
