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

#include "VariablesSet.h"

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
        int squaresCount;
        int sectorsCount;
        std::string id;
        
        DifficultInfo()
        : sectorsCount(0)
        , squaresCount(0)
        {}
    };
    
    struct ProjectileType
    {
        float healthDamage;
        float staminaDamage;
        float speed;
        float lifeTime;
        std::string texture;
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
        enum Melle
        {
            SAME_LINE,
            LEFT_LINE,
            RIGHT_LINE
        };
        
        struct RangeAttack
        {
            float distance;
            float recoveryTime;
            bool allowed;
            ProjectileType projectile;
            
            RangeAttack()
            : allowed(false)
            {}
        };
        
        struct MelleAttack
        {
            Melle condition;
            float areaWidth;
            float areaHeight;
            float dmgHealth;
            float dmgStamina;
            float showHighlightRange;
            float showHighlightTime;
            float recoveryTime;
            bool mustShowHighlight;
            bool allowed;
            
            MelleAttack()
            : allowed(false)
			, areaWidth(0.0f)
			, areaHeight(0.0f)
            {}
        };
        
        float damage;
        float health;
        float staminaDrainPoints;
        
        int killPoints;
        int goldPointsReward;
        int scorePointsReward;
        int staminaPointsReward;
        
        RangeAttack rangeAttack;
        MelleAttack melleAttack;
        
        std::string sprite;
        std::string model;
        int colorR, colorG, colorB;
        
        EnemyType()
        {}
    };
    
    typedef std::vector<DifficultInfo> DiffucultSettings;
    typedef std::vector<std::string> GameplayObjectsTypes;
    typedef std::map<std::string, ObstacleType> ObstaclesSettings;
    typedef std::map<std::string, EnemyType> EnemiesSettings;
    
public:
    static GameInfo& getInstance();
    
    bool loadInfo(const std::string &filename);
    bool loadSectors(const std::string &filename);
    
    int getInt(const std::string &name, int def = 0) const;
    bool getBool(const std::string &name, bool def = false) const;
    float getFloat(const std::string &name, float def = 0.0f) const;
    std::string getString(const std::string &name, std::string def = "") const;
    
    void setInt(const std::string &name, int value);
    void setBool(const std::string &name, bool value);
    void setFloat(const std::string &name, float value);
    void setString(const std::string &name, const std::string &value);
    
    const ObstacleType& getObstacleInfoByName(const std::string &name) const;
    const EnemyType& getEnemyInfoByName(const std::string &name) const;
    
    GameplayObjectsTypes getObstaclesTypes() const;
    GameplayObjectsTypes getEnemiesTypes() const;
    
    const DiffucultSettings& getDiffucultSettings() const;
    
    DifficultInfo getDifficultForSector(int sector) const;
    
private:
    VariablesSet _variables;
    DiffucultSettings _diffucultSettings;
    ObstaclesSettings _obstaclesSettings;
    EnemiesSettings _enemiesSettings;
};

#endif /* defined(__SlashingTrough__GameInfo__) */
