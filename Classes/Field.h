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
#include "FieldSector.h"
#include "Hero.h"

class Field : public ModelBase
{
public:
    Field();
    ~Field();
    
    void initialize();
    void finalize();
    
    void idleUpdate(float dt);
    
    void pushFrontSector();
    void popBackSector();
    void addEntity(Entity *entity);
    void updateDifficult();
    
    FieldSector::Ptr getSectorByUid(Uid uid);
    Entity* getEntityByUid(Uid uid);
    Hero* getHero() const;
    
private:
    typedef std::list<FieldSector::Ptr> SectorsSequence;
    
private:
    SectorsSequence _sectors;
    Hero *_hero;
    Entities _entities;
    GameInfo::DifficultInfo _difficult;
    
    int _passedSectors;
    int _difficultIndex;
    int _defaultSectorYSquares;
    int _squareSize;
};

#endif /* defined(__SlashingTrough__Field__) */
