//
//  PathSector.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 04/04/15.
//
//

#include "PathSector.h"

PathSector::Ptr PathSector::Create()
{
    return std::make_shared<PathSector>();
}

PathSector::PathSector()
{
    Reset();
}

void PathSector::Generate(int obstacles, int enemies, int squaresByHeight)
{
    _countObstacles = obstacles;
    _countEnemies = enemies;
    _squaresByHeight = squaresByHeight;
    
    const int col = 3;
    const int row = _squaresByHeight;
    
    _grid.resize(row * col);
    
    std::function<void(int, int)> spawnObstacle = [&](int x, int y)
    {
        GameInfo &gameInfo = GameInfo::Instance();
        
        GameInfo::GameplayObjectsTypes obstacleTypes;
        obstacleTypes = gameInfo.GetObstaclesTypes();
        
        GameInfo::ObstacleType info;
        info = gameInfo.GetObstacleInfoByName(obstacleTypes[0]);
        
        float square = gameInfo.GetFloat("SQUARE_SIZE");
        float localX = x * square + square * 0.5f;
        float localY = y * square + square * 0.5f;
        
        Obstacle::Ptr obj;
        obj = Obstacle::Create(info);
        obj->SetLogicalPos(localX, localY);
        _objects.push_back(obj);
        
        int index = x * _squaresByHeight + y;
        _grid[index].state = Square::State::OBSTACLE;
    };
    
    std::function<void(int, int)> spawnEnemies = [&](int x, int y)
    {
        GameInfo &gameInfo = GameInfo::Instance();
        
        GameInfo::GameplayObjectsTypes enemiesTypes;
        enemiesTypes = gameInfo.GetEnemiesTypes();
        
        GameInfo::EnemyType info;
        info = gameInfo.GetEnemyInfoByName(enemiesTypes[0]);
        
        float square = gameInfo.GetFloat("SQUARE_SIZE");
        float localX = x * square + square * 0.5f;
        float localY = y * square + square * 0.5f;
        
        Enemy::Ptr obj;
        obj = Enemy::Create(info);
        obj->SetLogicalPos(localX, localY);
        _objects.push_back(obj);
        
        int index = x * _squaresByHeight + y;
        _grid[index].state = Square::State::ENEMY;
    };
    
    SpawnObjects(spawnObstacle, obstacles);
    SpawnObjects(spawnEnemies, enemies);
}

void PathSector::Reset()
{
    _countObstacles = 0;
    _countEnemies = 0;
    _squaresByHeight = 0;
    _objects.clear();
    _grid.clear();
}

const PathSector::GameplayObjects& PathSector::GetGameplayObjects() const
{
    return _objects;
}

void PathSector::SpawnObjects(const std::function<void(int, int)> &spawn, int amount)
{
    const int col = 3;
    const int row = _squaresByHeight;
    
    int generated = 0;
    while (generated < amount) {
        int yidx = rand() % row;
        if (IsValidRow(yidx)) {
            int xidx = rand() % col;
            bool allowGenerate = true;
            while (allowGenerate) {
                spawn(xidx, yidx);
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
                    if (yidx >= row || !IsValidRow(yidx)) {
                        allowGenerate = false;
                    }
                }
            }
        }
    }
}

bool PathSector::IsValidRow(int row) const
{
    for (int k = 0; k < 3; ++k) {
        // x * h + y
        int index = k * _squaresByHeight + row;
        if (_grid[index].state != Square::State::FREE) {
            return false;
        }
    }
    return true;
}