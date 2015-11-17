//
//  Field.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 11/07/15.
//
//

#ifndef __SlashingTrough__Field__
#define __SlashingTrough__Field__

#include <stdio.h>
#include <list>

#include "GameInfo.h"
#include "FieldLevel.h"
#include "Hero.h"

class Field : public ModelBase
{
public:
    Field();
    ~Field();
    
    void initialize(FieldLevel::WeakPtr level);
    void finalize();
    
    void idleUpdate(float dt);
    
    void pushFrontSector();
    void popBackSector();
    void addEntity(Entity *entity);
    
    FieldLevel::Ptr getLevel();
    FieldSector::Ptr getSectorByUid(Uid uid);
    FieldSector::Ptr getCurrentSector();
    Entity* getEntityByUid(Uid uid);
    Hero* getHero() const;
    int getPassedSectors() const;
    
private:
    typedef std::list<FieldSector::Ptr> SectorsSequence;
    
private:
    FieldLevel::Ptr _level;
    SectorsSequence _sectors;
    Hero *_hero;
    Entities _entities;
    
    float  _heroLastYPos;
    
    int _passedSectors;
    int _defaultSectorYSquares;
    int _squareSize;
    
    bool _finished;
};

#endif /* defined(__SlashingTrough__Field__) */
