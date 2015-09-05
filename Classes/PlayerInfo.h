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

#include "Equipment.h"
#include "VariablesSet.h"

class PlayerInfo
{
public:
    static const std::string VarKeyCoins;
    static const std::string VarKeyEquipWpn;
    static const std::string VarKeyEquipArm;
    
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
    
    void addOwnedEquip(Equip::Ptr item);
    void addCoins(int coins);
    void equip(Equip::Ptr item);

    int getCoins() const;
    int getDamage() const;
    bool isEquipOwned(Equip::Ptr item) const;
    bool isEquipped(Equip::Ptr item) const;
    
    VariablesSet variables;
    
private:
    std::vector<std::string> _ownedEquips;
    std::string _saveFileName;
};

#endif /* defined(__SlashingTrough__PlayerInfo__) */
