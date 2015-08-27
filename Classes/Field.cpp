//
//  Field.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 11/07/15.
//
//

#include "Field.h"
#include "Obstacle.h"
#include "Enemy.h"
#include "Log.h"

Field::Field()
: ModelBase()
, _passedSectors(0)
, _hero(nullptr)
{
    
}

Field::~Field()
{
    finalize();
}

void Field::initialize(FieldLevel::WeakPtr level)
{
    if (level.expired()) {
        WRITE_WARN("Failed to start game with invalid level");
        return;
    }
    
    _level = level.lock();
    _level->rebuild();
    
    GameInfo &gameinfo = GameInfo::getInstance();
    
    _defaultSectorYSquares = gameinfo.getInt("DEFAULT_SEСTOR_SQUARES_COUNT");
    _squareSize = gameinfo.getFloat("SQUARE_SIZE");

    float heroStartX = _squareSize * 3.0f * 0.5f;
    float heroStartY = 0.0f;
    
    _hero = new Hero();
    _hero->setPosition(heroStartX, heroStartY);
    _hero->setRunning(true);
    //_hero->setRunningSpeed(0.0f);
    _hero->setSideBorders(0.0f, _squareSize * 3.0f);
    _heroLastYPos = _hero->getPositionY();
    
    int sectorsQueueSize = gameinfo.getInt("SECTORS_SEQUENCE_MAX_SIZE");
    for (int k = 0; k < sectorsQueueSize; ++k) {
        pushFrontSector();
    }
    
    WRITE_LOG("Game started with level:" + _level->getId());
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
    if (_level->getSectorByIndex(_passedSectors)) {
        auto sector = _level->getSectorByIndex(_passedSectors);
        float speed = sector->getRunningSpeed();
        _hero->setRunningSpeed(speed);
    }
    
    for (auto iter = _sectors.begin(); iter != _sectors.end();)
    {
        FieldSector::Ptr sector = (*iter);
        float heroY = _hero->getPositionY();
        float sectorH = sector->getHeight();
        float sectorY = sector->getY();
        if (sectorY < heroY && sectorY > _heroLastYPos) {
            _heroLastYPos = heroY;
            _passedSectors++;
        }
        if ((sectorY + sectorH * 2.0f) < heroY)
        {
            // first of all delete sector widget
            Event e("SectorDeleted");
            e.variables.setInt("uid", sector->getUid());
            sendEvent(e);
            
            iter = _sectors.erase(iter);
            pushFrontSector();
        } else
            ++iter;
    }
    
    for (auto iter = _entities.begin(); iter != _entities.end();)
    {
        Entity *entity = (*iter);
        if (entity->isAlive()) {
            int sectorHeight = _defaultSectorYSquares * _squareSize;
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

void Field::pushFrontSector()
{
    auto sector = _level->getNextSector();
    
    if (sector)
    {
        GameInfo &gameinfo = GameInfo::getInstance();
        
        auto content = sector->getContent();
        // initialize entities
        for (auto info : content) {
            Entity *entity = nullptr;
            switch (info.type) {
                case Entity::Type::OBSTACLE:
                    entity = new Obstacle(gameinfo.getObstacleInfoByName(info.name));
                    break;
                case Entity::Type::ENEMY:
                    entity = new Enemy(gameinfo.getEnemyInfoByName(info.name), this);
                    break;
                default:
                    break;
            }
            if (entity) {
                entity->setPositionX(info.x * _squareSize + _squareSize * 0.5f);
                entity->setPositionY(info.y * _squareSize + _squareSize * 0.5f + sector->getY());
                entity->setGoal(_hero);
                addEntity(entity);
            }
        }
    
        _sectors.push_back(sector);
    
        Event e("SectorAdded");
        e.variables.setInt("uid", sector->getUid());
        sendEvent(e);
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

FieldSector::Ptr Field::getSectorByUid(Uid uid)
{
    for (const auto sector : _sectors) {
        if (sector->getUid() == uid) {
            return sector;
        }
    }
    return FieldSector::Ptr();
}

FieldSector::Ptr Field::getCurrentSector()
{
    return _level->getSectorByIndex(_passedSectors);
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

int Field::getPassedSectors() const
{
    return _passedSectors;
}
