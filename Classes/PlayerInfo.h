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

class PlayerInfo
{
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
    void setBestScore(const Score &score);
    void setLastCompletedLevelId(const std::string &id);
    void setLastIncompletedLevelId(const std::string &id);

    Score getBestScore() const;
    int getCoins() const;
    int getDamage() const;
    bool isBestScore(const Score &score) const;
    bool isEquipOwned(Equip::Ptr item) const;
    bool isEquipped(Equip::Ptr item) const;
    
    const std::string& getLastCompletedLevelId() const;
    const std::string& getLastIncompletedLevelId() const;
    const std::string& getEquippedWeaponId() const;
    const std::string& getEquippeArmorId() const;
    
private:
    typedef std::vector<std::string> OwnedEquipIds;
    
private:
    int _totalCoins;
    
    OwnedEquipIds _ownedEquips;
    Score _bestScore;
    
    std::string _lastCompletedLevelId;
    std::string _lastIncompletedLevelId;
    std::string _equippedWeaponId;
    std::string _equippedArmorId;
    std::string _saveFileName;
};

#endif /* defined(__SlashingTrough__PlayerInfo__) */
