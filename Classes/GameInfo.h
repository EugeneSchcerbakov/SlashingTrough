//
//  GameInfo.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 01/04/15.
//
//

#ifndef __SlashingTrough__GameInfo__
#define __SlashingTrough__GameInfo__

#include <vector>
#include <string>
#include <map>

class GameInfo
{
public:
    struct SpawnInfo
    {
        std::string name;
        int amount;
        
        SpawnInfo()
        : name("")
        , amount(0)
        {}
    };
    
    struct DifficultInfo
    {
        typedef std::vector<SpawnInfo> SpawnList;
        
        SpawnList enemiesPerSector;
        SpawnList obstaclesPerSector;
        float speed;
        int sectors;
        
        DifficultInfo()
        : sectors(0)
        {}
    };
    
    struct ObstacleType
    {
        float health;
        float damage;
        float staminaDrainPoints;
        bool destructible;
        
        int killPoints;
        int goldPointsReward;
        int scorePointsReward;
        int staminaPointsReward;
        
        std::string sprite;
        
        ObstacleType() {}
    };
    
    struct EnemyType
    {
        float damage;
        float health;
        float staminaDrainPoints;
        
        int killPoints;
        int goldPointsReward;
        int scorePointsReward;
        int staminaPointsReward;
        
        std::string sprite;
        
        EnemyType() {}
    };
    
    typedef std::vector<DifficultInfo> DiffucultSettings;
    typedef std::vector<std::string> GameplayObjectsTypes;
    typedef std::map<std::string, ObstacleType> ObstaclesSettings;
    typedef std::map<std::string, EnemyType> EnemiesSettings;
    
public:
    static GameInfo& Instance();
    
    bool LoadInfo(const std::string &filename);
    
    int GetInt(const std::string &name, int def = 0) const;
    bool GetBool(const std::string &name, bool def = false) const;
    float GetFloat(const std::string &name, float def = 0.0f) const;
    std::string GetString(const std::string &name, std::string def = "") const;
    
    void SetInt(const std::string &name, int value);
    void SetBool(const std::string &name, bool value);
    void SetFloat(const std::string &name, float value);
    void SetString(const std::string &name, const std::string &value);
    
    const ObstacleType& GetObstacleInfoByName(const std::string &name) const;
    const EnemyType& GetEnemyInfoByName(const std::string &name) const;
    
    GameplayObjectsTypes GetObstaclesTypes() const;
    GameplayObjectsTypes GetEnemiesTypes() const;
    
    const DiffucultSettings& GetDiffucultSettings() const;
    
private:
    std::map<std::string, int> _variablesInt;
    std::map<std::string, float> _variablesFloat;
    std::map<std::string, std::string> _variablesStr;
    
    DiffucultSettings _diffucultSettings;
    ObstaclesSettings _obstaclesSettings;
    EnemiesSettings _enemiesSettings;
};

#endif /* defined(__SlashingTrough__GameInfo__) */
