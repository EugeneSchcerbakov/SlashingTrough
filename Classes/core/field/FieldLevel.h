//
//  FieldLevel.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 08/08/15.
//
//

#ifndef __SlashingTrough__FieldLevel__
#define __SlashingTrough__FieldLevel__

#include "FieldSector.h"
#include "VictoryCondition.h"
#include "Item.h"

#include "tinyxml2/tinyxml2.h"

class FieldLevel
{
public:
    typedef std::shared_ptr<FieldLevel> Ptr;
    typedef std::weak_ptr<FieldLevel> WeakPtr;
    
    enum class Status
    {
        LOCKED,
        UNLOCKED,
        COMPLETED
    };
    
    struct Drop
    {
        std::string itemId;
        int chance;
        bool once;
        bool droped;
        Drop()
        : chance(0)
        , once(true)
        , droped(false)
        {}
    };
    
    struct SaveData
    {
        // item, flag
        typedef std::pair<std::string, bool> DropFact;
        
        Status status;
        std::vector<DropFact> occurredDrop;
        
        SaveData() = default;
    };
    
    static FieldLevel::Ptr create();
    static Status stringToStatus(const std::string &str);
    static std::string statusToString(Status status);
    
public:
    FieldLevel();
    
    void initFromXml(tinyxml2::XMLNode *node);
    void prepearForRun(Hero *hero);
    void restore(const SaveData &saveData);
    void update(float dt);
    void release();
    void setStatus(Status status);
    void setPosOnMap(float x, float y);
    
    FieldSector::Ptr getNextSector();
    FieldSector::Ptr getSectorByIndex(int index);
    
    const std::string& getId() const;
    const std::vector<std::string>& getUnlocks() const;
    
    std::vector<std::string> getPossibleLoot() const;
    std::vector<std::string> dropLoot();
    
    Status getStatus() const;
    SaveData getSaveData() const;
    int getCoinsReward() const;
    int getSectorsAmount() const;
    float getMapX() const;
    float getMapY() const;
    float getRunningTime() const;
    float getRinningSpeedForPos(float ypos) const;
    bool isStatus(Status status);
    bool isFinished() const;
    
    VictoryCondition::WeakPtr getVictoryCondition() const;
    
private:
    struct ConstructionInfo
    {
        int enemiesAmount;
        int lengthInSquares;
        float runningSpeedBegin;
        float runningSpeedEnd;
        float enemiesDifficultCoeff;
        
        std::vector<std::string> abaliableEnemies;
        
        ConstructionInfo() = default;
    };
    
private:
    void addSector(const Preset &preset);
    void mirrorPreset(Preset &preset);
    bool isSuitablePreset(const Preset &preset) const;
    
    int _lastSectorIndex;
    int _coinRewardForCompletition;
    float _posOnMapX;
    float _posOnMapY;
    float _runnigTimeSec;
    
    Status _status;
    ConstructionInfo _info;
    VictoryCondition::Ptr _victoryCondition;
    
    std::string _id;
    std::vector<FieldSector::Ptr> _sectors;
    std::vector<std::string> _unlocks; // id's of unlocked levels after complete
    std::vector<Drop> _drops;
};

#endif /* defined(__SlashingTrough__FieldLevel__) */
