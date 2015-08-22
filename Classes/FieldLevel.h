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
        Drop()
        : chance(0)
        , once(true)
        {}
    };
    
    static FieldLevel::Ptr create();
    static Status stringToStatus(const std::string &str);
    
public:
    FieldLevel();
    
    void initFromXml(tinyxml2::XMLNode *node);
    void release();
    
    FieldSector::Ptr getNextSector();
    FieldSector::Ptr getSectorByIndex(int index);
    
    float getPosOnMapX() const;
    float getPosOnMapY() const;
    const std::string& getId() const;
    const std::vector<std::string> getUnlocks() const;
    
    bool isStatus(Status status);
    
private:
    void addSector(const Preset &preset, float speed);
    
    float _posOnMapX;
    float _posOnMapY;
    
    int _lastSectorIndex;
    int _coinRewardForCompletition;
    
    Status _status;
    
    std::string _id;
    std::vector<FieldSector::Ptr> _sectors;
    std::vector<std::string> _unlocks; // id's of unlocked levels after complete
    std::vector<Drop> _drops;
};

#endif /* defined(__SlashingTrough__FieldLevel__) */
