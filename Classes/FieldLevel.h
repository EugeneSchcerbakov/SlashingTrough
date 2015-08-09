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
    
    static FieldLevel::Ptr create();
    
public:
    FieldLevel();
    
    void initFromXml(tinyxml2::XMLNode *node);
    void release();
    
    FieldSector::Ptr getNextSector();
    FieldSector::Ptr getSectorByIndex(int index);
    
    const std::string& getId() const;
    
private:
    void addSector(const Preset &preset, float speed);
    
    int _lastSectorIndex;
    
    std::string _id;
    std::vector<FieldSector::Ptr> _sectors;
};

#endif /* defined(__SlashingTrough__Level__) */
