//
//  PathSector.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 04/04/15.
//
//

#ifndef __SlashingTrough__PathSector__
#define __SlashingTrough__PathSector__

#include <stdlib.h>
#include <cmath>
#include <memory>
#include <functional>

#include "GameplayObjects.h"

class PathSector
{
public:
    typedef std::shared_ptr<PathSector> Ptr;
    typedef std::weak_ptr<PathSector> WeakPtr;
    typedef std::vector<GameplayObject::Ptr> GameplayObjects;
    typedef GameplayObjects::iterator GameplayObjectsIter;
    
    struct Square
    {
        enum class State
        {
            FREE,
            OBSTACLE,
            ENEMY
        };
        
        GameplayObject::UID objUID;
        State state;
        int x, y;
        
        Square()
        : objUID(GameplayObject::InvalidUID)
        , state(State::FREE)
        , x(-1), y(-1)
        {}
    };

public:
    static Ptr Create();
    
    PathSector();
    
    void Generate(int obstacles, int enemies, int squaresByHeight);
    void Reset();
    
    Square GetSquareByLocalXY(float x, float y) const;
    Square GetSquareByIndexXY(int x, int y) const;
    Square GetSquareByObject(GameplayObject::Ptr object) const;
    
    inline int GetObstaclesCount() const {return _countObstacles;}
    inline int GetEmeniesCount() const {return _countEnemies;}
    inline int GetSquaresByHeight() const {return _squaresByHeight;}
    
    GameplayObject::WeakPtr GetObjectByUID(int uid);
    GameplayObjects& GetGameplayObjects();
    
    bool IsValidSquareAddress(int x, int y) const;
    
private:
    typedef std::vector<Square> Grid;
    
private:
    void SpawnObjects(const std::function<void(int, int)> &spawn, int amount);
    bool IsValidRow(int row) const;
    GameplayObject::UID GenerateUID();
    
    Grid _grid;
    GameplayObjects _objects;
    
    const float _squareSize;
    
    int _countObstacles;
    int _countEnemies;
    int _squaresByHeight;
};

#endif /* defined(__SlashingTrough__PathSector__) */
