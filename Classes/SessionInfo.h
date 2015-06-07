//
//  SessionInfo.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 07/06/15.
//
//

#ifndef __SlashingTrough__SessionInfo__
#define __SlashingTrough__SessionInfo__

#include <string>

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
    static SessionInfo& Instance();
    
    SessionInfo();
    
    void Load(const std::string &filename);
    void Save();
    
    void AddOwnedEquip(const std::string id);
    void AddCoins(int coins);
    void SetBestScore(const Score &score);
    
    Score GetBestScore();
    int GetCoins() const;
    bool IsBestScore(const Score &score) const;
    bool IsEquipOwned(const std::string &id);
    
private:
    typedef std::vector<std::string> OwnedEquipIds;
    
private:
    int _totalCoins;
    
    OwnedEquipIds _ownedEquips;
    Score _bestScore;
    std::string _saveFileName;
};

#endif /* defined(__SlashingTrough__SessionInfo__) */
