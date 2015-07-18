//
//  FieldSector.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 11/07/15.
//
//

#include "FieldSector.h"
#include "Obstacle.h"
#include "Enemy.h"
#include "Field.h"

#include <stdlib.h>
#include <math.h>

FieldSector::Ptr FieldSector::create()
{
    return std::make_shared<FieldSector>();
}

FieldSector::FieldSector()
: ModelBase()
, _width(0)
, _height(0)
, _logicX(0)
, _logicY(0)
, _squaresByWidth(0)
, _squaresByHeight(0)
{
}

Entities FieldSector::generate(int squaresByHeight, const GameInfo::DifficultInfo difficult, Field *field)
{
    GameInfo &gameinfo = GameInfo::getInstance();
    
    _squaresByWidth = 3;
    _squaresByHeight = squaresByHeight;
    _squareSize = gameinfo.getFloat("SQUARE_SIZE");
    _width = _squaresByWidth * _squareSize;
    _height = _squaresByHeight * _squareSize;
    
    _grid.resize(_squaresByWidth * _squaresByHeight);
    
    for (int i = 0; i < _squaresByWidth; i++) {
        for (int j = 0; j < _squaresByHeight; j++) {
            int index = i * _squaresByHeight + j;
            _grid[index].x = i;
            _grid[index].y = j;
        }
    }
    
    ConstructionMap map;
    for (auto spawn : difficult.obstaclesPerSector) {
        generateConstructionMap(map, spawn, Entity::Type::OBSTACLE);
    }
    for (auto spawn : difficult.enemiesPerSector) {
        generateConstructionMap(map, spawn, Entity::Type::ENEMY);
    }
    
    Entities entities;
    for (auto info : map) {
        Entity *entity = nullptr;
        switch (info.type) {
            case Entity::Type::OBSTACLE:
                entity = new Obstacle(gameinfo.getObstacleInfoByName(info.name));
                break;
            case Entity::Type::ENEMY:
                entity = new Enemy(gameinfo.getEnemyInfoByName(info.name), field);
                break;
            default:
                break;
        }
        if (entity) {
            entity->setPositionX(info.square.x * _squareSize + _squareSize * 0.5f);
            entity->setPositionY(info.square.y * _squareSize + _squareSize * 0.5f);
            entities.push_back(entity);
        }
    }
    
    return entities;
}

void FieldSector::reset()
{
    _grid.clear();
}

void FieldSector::setX(float x)
{
    _logicX = x;
}

void FieldSector::setY(float y)
{
    _logicY = y;
}

void FieldSector::setSquareFree(int x, int y, bool flag)
{
    if (isValidSquareAddress(x, y)) {
        int index = x * _squaresByHeight + y;
        _grid[index].free = flag;
    }
}

Square FieldSector::getSquareByLogicXY(float x, float y) const
{
    int xidx = (int)floorf(x / _squareSize);
    int yidx = (int)floorf(y / _squareSize);
    
    return getSquareByIndexXY(xidx, yidx);
}

Square FieldSector::getSquareByIndexXY(int x, int y) const
{
    if (isValidSquareAddress(x, y)) {
        int index = x * _squaresByHeight + y;
        return _grid[index];
    }
    return Square();
}

float FieldSector::getWidth() const
{
    return _width;
}

float FieldSector::getHeight() const
{
    return _height;
}

float FieldSector::getX() const
{
    return _logicX;
}

float FieldSector::getY() const
{
    return _logicY;
}

int FieldSector::getSquaresNumByX() const
{
    return _squaresByWidth;
}

int FieldSector::getSquaresNumByY() const
{
    return _squaresByHeight;
}

bool FieldSector::isValidSquareAddress(int x, int y) const
{
    return x >= 0 && x < _squaresByWidth && y >= 0 && y < _squaresByHeight;
}

bool FieldSector::isValidRow(int row) const
{
    for (int k = 0; k < 3; ++k) {
        // x * h + y
        int index = k * _squaresByHeight + row;
        if (!_grid[index].free) {
            return false;
        }
    }
    return true;
}

void FieldSector::generateConstructionMap(ConstructionMap &map, const GameInfo::SpawnInfo &spawn, Entity::Type type)
{
    const int col = 3;
    const int row = _squaresByHeight;
    const int amount = spawn.amount;
    
    int generated = 0;
    while (generated < amount) {
        int yidx = rand() % row;
        if (isValidRow(yidx)) {
            int xidx = rand() % col;
            bool allowGenerate = true;
            while (allowGenerate) {
                auto info = ConstructionInfo();
                info.square.x = xidx;
                info.square.y = yidx;
                info.name = spawn.name;
                info.type = type;
                map.push_back(info);
                setSquareFree(xidx, yidx, false);
                
                ++generated;
                if (generated >= amount) {
                    break;
                }
                
                if (rand() % 100 < 10) {
                    // broke the objects chain
                    allowGenerate = false;
                } else {
                    // continue objects chain
                    yidx += 1;
                    int nextIdxX = xidx;
                    do {
                        nextIdxX = rand() % col;
                    } while (nextIdxX == xidx);
                    xidx = nextIdxX;
                    // validate row
                    if (yidx >= row || !isValidRow(yidx)) {
                        allowGenerate = false;
                    }
                }
            }
        }
    }
}
