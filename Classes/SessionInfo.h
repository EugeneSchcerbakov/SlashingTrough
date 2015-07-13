//
//  SessionInfo.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 07/06/15.
//
//

#ifndef __SlashingTrough__SessionInfo__
#define __SlashingTrough__SessionInfo__

#include <vector>
#include <string>

#include "Equipment.h"

class SessionInfo
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
    static SessionInfo& getInstance();
    
    SessionInfo();
    
    void load(const std::string &filename);
    void save();
    
    void addOwnedEquip(const std::string id);
    void addCoins(int coins);
    void equipWeapon(Equip::WeakPtr weapon);
    void setBestScore(const Score &score);
    
    Score getBestScore() const;
    int getCoins() const;
    bool isBestScore(const Score &score) const;
    bool isEquipOwned(const std::string &id) const;
    bool isWeaponEquipped(const std::string &id) const;
    
    const std::string& getEquippedWeaponId() const;
    
private:
    typedef std::vector<std::string> OwnedEquipIds;
    
private:
    int _totalCoins;
    
    OwnedEquipIds _ownedEquips;
    Score _bestScore;
    std::string _equippedWeaponId;
    std::string _saveFileName;
};

#endif /* defined(__SlashingTrough__SessionInfo__) */
