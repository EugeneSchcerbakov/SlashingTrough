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

#include "ModelBase.h"
#include "VariablesSet.h"

class GameInfo
{
public:
    struct ProjectileType
    {
        float healthDamage;
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
    
    typedef std::vector<std::string> GameplayObjectsTypes;
    typedef std::map<std::string, ObstacleType> ObstaclesSettings;
    typedef std::map<std::string, EnemyType> EnemiesSettings;
    
public:
    static GameInfo& getInstance();
    
    bool loadInfo(const std::string &filename);
    
    int getConstInt(const std::string &name, int def = 0) const;
    bool getConstBool(const std::string &name, bool def = false) const;
    float getConstFloat(const std::string &name, float def = 0.0f) const;
    std::string getConstString(const std::string &name, std::string def = "") const;
    
    int getGlobalInt(const std::string &name, int def = 0) const;
    bool getGlobalBool(const std::string &name, bool def = false) const;
    float getGlobalFloat(const std::string &name, float def = 0.0f) const;
    std::string getGlobalString(const std::string &name, std::string def = "") const;
    
    void setGlobalInt(const std::string &name, int value);
    void setGlobalBool(const std::string &name, bool value);
    void setGlobalFloat(const std::string &name, float value);
    void setGlobalString(const std::string &name, const std::string &value);
    
    Entity::Type getTypeById(const std::string &id) const;
    
    const ObstacleType& getObstacleInfoByName(const std::string &name) const;
    const EnemyType& getEnemyInfoByName(const std::string &name) const;
    
    GameplayObjectsTypes getObstaclesTypes() const;
    GameplayObjectsTypes getEnemiesTypes() const;
    
private:
    VariablesSet _constants;
    VariablesSet _globals;
    ObstaclesSettings _obstaclesSettings;
    EnemiesSettings _enemiesSettings;
};

#endif /* defined(__SlashingTrough__GameInfo__) */
