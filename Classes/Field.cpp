//
//  Field.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 11/07/15.
//
//

#include "Field.h"

Field::Field()
: ModelBase()
, _passedSectors(0)
, _difficultIndex(0)
, _hero(nullptr)
{
    
}

Field::~Field()
{
    finalize();
}

void Field::initialize()
{
    GameInfo &gameinfo = GameInfo::getInstance();
    
    if (!gameinfo.getDiffucultSettings().empty()) {
        _difficultIndex = 0;
        _difficult = gameinfo.getDiffucultSettings()[_difficultIndex];
    }
    
    _sectorSquaresByY = gameinfo.getInt("SEСTOR_SQUARES_COUNT");
    _squareSize = gameinfo.getFloat("SQUARE_SIZE");

    float heroStartX = _squareSize * 3.0f * 0.5f;
    float heroStartY = _sectorSquaresByY * _squareSize; // leave one sector behind
    
    _hero = new Hero();
    _hero->setPosition(heroStartX, heroStartY);
    _hero->setRunning(true);
    _hero->setRunningSpeed(_difficult.speed);
    _hero->setSideBorders(0.0f, _squareSize * 3.0f);
    
    int sectorsQueueSize = gameinfo.getInt("SECTORS_SEQUENCE_MAX_SIZE");
    for (int k = 0; k < sectorsQueueSize; ++k) {
        bool empty = k <= 1;
        pushFrontSector(empty);
    }
}

void Field::finalize()
{
    if (_hero) {
        delete _hero;
        _hero = nullptr;
    }
    
    for (auto entity : _entities) {
        delete entity;
        entity = nullptr;
    }
    
    _entities.clear();
    _sectors.clear();
}

void Field::idleUpdate(float dt)
{
    if (_hero && !_hero->isAlive()) {
        // first of all delete hero widget
        Event e("HeroKilled");
        e.variables.setInt("uid", _hero->getUid());
        sendEvent(e);
        
        delete _hero;
        _hero = nullptr;
        return;
    }
    
    _hero->idleUpdate(dt);
    _hero->refreshGoals(&_entities);
    _hero->setRunningSpeed(_difficult.speed);
    
    for (auto iter = _sectors.begin(); iter != _sectors.end();)
    {
        FieldSector::Ptr sector = (*iter);
        float heroY = _hero->getPositionY();
        float sectorY = sector->getY() + sector->getHeight() * 2.0f;
        if (sectorY < heroY)
        {
            // first of all delete sector widget
            Event e("SectorDeleted");
            e.variables.setInt("uid", sector->getUid());
            sendEvent(e);
            
            ++_passedSectors;
            iter = _sectors.erase(iter);
            pushFrontSector();
            updateDifficult();
        } else
            ++iter;
    }
    
    for (auto iter = _entities.begin(); iter != _entities.end();)
    {
        Entity *entity = (*iter);
        if (entity->isAlive()) {
            int sectorHeight = _sectorSquaresByY * _squareSize;
            float entityY = entity->getPositionY();
            float heroY = _hero->getPositionY();
            if (entityY + sectorHeight < heroY) {
                entity->kill();
            } else {
                entity->idleUpdate(dt);
            }
            ++iter;
        } else {
            // first of all delete entity widget
            Event e("EntityDeleted");
            e.variables.setInt("uid", entity->getUid());
            sendEvent(e);
            
            delete entity;
            iter = _entities.erase(iter);
        }
    }
}

void Field::pushFrontSector(bool empty)
{
    GameInfo::DifficultInfo difficult;
    if (!empty) {
        difficult = _difficult;
    } else {
        // make empty
        difficult.obstaclesPerSector.clear();
        difficult.enemiesPerSector.clear();
    }
    
    FieldSector::Ptr sector = FieldSector::create();
    auto content = sector->generate(_sectorSquaresByY, difficult, this);
    
    // place sector to field
    float ypos = 0.0f;
    if (!_sectors.empty()) {
        SectorsSequence::const_iterator last = --_sectors.end();
        float lastYPos = (*last)->getY();
        ypos = lastYPos + sector->getHeight();
    }
    
    sector->setX(0.0f);
    sector->setY(ypos);
    
    _sectors.push_back(sector);
    
    Event e("SectorAdded");
    e.variables.setInt("uid", sector->getUid());
    sendEvent(e);
    
    // initialize entities
    for (auto entity : content) {
        // convert pos from sector to field space
        float y = entity->getPositionY();
        entity->setPositionY(ypos + y);
        entity->setGoal(_hero);
        addEntity(entity);
    }
}

void Field::popBackSector()
{
    
}

void Field::addEntity(Entity *entity)
{
    if (!entity) {
        return;
    }
    
    if (!getEntityByUid(entity->getUid()))
    {
        _entities.push_back(entity);
        
        Event e("EntityAdded");
        e.variables.setInt("uid", entity->getUid());
        sendEvent(e);
    }
}

void Field::updateDifficult()
{
    GameInfo &gameinfo = GameInfo::getInstance();
    const GameInfo::DiffucultSettings &settings = gameinfo.getDiffucultSettings();
    if (_passedSectors >= _difficult.sectors) {
        _passedSectors = 0;
        ++_difficultIndex;
        if ((std::size_t)_difficultIndex < settings.size()) {
            _difficult = settings[_difficultIndex];
        }
    }
}

FieldSector::Ptr Field::getSectorByUid(Uid uid)
{
    for (const auto sector : _sectors) {
        if (sector->getUid() == uid) {
            return sector;
        }
    }
    return FieldSector::Ptr();
}

Entity* Field::getEntityByUid(Uid uid)
{
    for (auto entity : _entities) {
        if (entity->getUid() == uid) {
            return entity;
        }
    }
    return nullptr;
}

Hero* Field::getHero() const
{
    return _hero;
}
