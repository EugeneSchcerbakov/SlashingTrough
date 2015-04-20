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
: _squareSize(GameInfo::Instance().GetFloat("SQUARE_SIZE"))
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
    
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < _squaresByHeight; ++j) {
            int index = i*row+j;
            _grid[index].x = i;
            _grid[index].y = j;
        }
    }
    
    std::function<void(int, int)> spawnObstacle = [&](int x, int y)
    {
        GameInfo &gameInfo = GameInfo::Instance();
        
        GameInfo::GameplayObjectsTypes obstacleTypes;
        obstacleTypes = gameInfo.GetObstaclesTypes();
        
        GameInfo::ObstacleType info;
        info = gameInfo.GetObstacleInfoByName(obstacleTypes[0]);
        
        float localX = x * _squareSize + _squareSize * 0.5f;
        float localY = y * _squareSize + _squareSize * 0.5f;
        
        GameplayObject::UID uid = GenerateUID();
        Obstacle::Ptr obj = Obstacle::Create(info, uid);
        obj->SetLogicalPos(localX, localY);
        _objects.push_back(obj);
        
        int index = x * _squaresByHeight + y;
        _grid[index].state = Square::State::OBSTACLE;
        _grid[index].objUID = uid;
    };
    
    std::function<void(int, int)> spawnEnemies = [&](int x, int y)
    {
        GameInfo &gameInfo = GameInfo::Instance();
        
        GameInfo::GameplayObjectsTypes enemiesTypes;
        enemiesTypes = gameInfo.GetEnemiesTypes();
        
        GameInfo::EnemyType info;
        info = gameInfo.GetEnemyInfoByName(enemiesTypes[0]);
        
        float localX = x * _squareSize + _squareSize * 0.5f;
        float localY = y * _squareSize + _squareSize * 0.5f;
        
        GameplayObject::UID uid = GenerateUID();
        Enemy::Ptr obj = Enemy::Create(info, uid);
        obj->SetLogicalPos(localX, localY);
        _objects.push_back(obj);
        
        int index = x * _squaresByHeight + y;
        _grid[index].state = Square::State::ENEMY;
        _grid[index].objUID = uid;
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

PathSector::Square PathSector::GetSquareByLocalXY(float x, float y) const
{
    int xidx = (int)floorf(x / _squareSize);
    int yidx = (int)floorf(y / _squareSize);
    
    return GetSquareByIndexXY(xidx, yidx);
}

PathSector::Square PathSector::GetSquareByIndexXY(int x, int y) const
{
    if (IsValidSquareAddress(x, y)) {
        int index = x * _squaresByHeight + y;
        return _grid[index];
    }
    return Square();
}

PathSector::Square PathSector::GetSquareByObject(GameplayObject::Ptr object) const
{
    float x = object->GetLogicalX();
    float y = object->GetLogicalY();
    
    Square square = GetSquareByLocalXY(x, y);
    if (IsValidSquareAddress(square.x, square.y)) {
        return square;
    }
    
    return Square();
}

GameplayObject::WeakPtr PathSector::GetObjectByUID(int uid)
{
    for (auto obj : _objects) {
        if (obj->GetUID() == uid) {
            return obj;
        }
    }
    return GameplayObject::Ptr();
}

PathSector::GameplayObjects& PathSector::GetGameplayObjects()
{
    return _objects;
}

bool PathSector::IsValidSquareAddress(int x, int y) const
{
    return x >= 0 && x < 3 && y >= 0 && y < _squaresByHeight;
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

GameplayObject::UID PathSector::GenerateUID()
{
    static GameplayObject::UID uid = 0;
    ++uid;
    return uid;
}

