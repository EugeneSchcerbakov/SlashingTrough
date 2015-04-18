//
//  PathSector.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 04/04/15.
//
//

#ifndef __SlashingTrough__PathSector__
#define __SlashingTrough__PathSector__

#include <memory>

#include "GameplayObjects.h"

class PathSector
{
public:
    typedef std::shared_ptr<PathSector> Ptr;
    typedef std::weak_ptr<PathSector> WeakPtr;
    typedef std::vector<GameplayObject::Ptr> GameplayObjects;
    
public:
    static Ptr Create();
    
    PathSector();
    
    void Generate(int obstacles, int enemies, int squaresByHeight);
    void Reset();
    
    inline int GetObstaclesCount() const {return _countObstacles;}
    inline int GetEmeniesCount() const {return _countEnemies;}
    inline int GetSquaresByHeight() const {return _squaresByHeight;}
    
    const GameplayObjects& GetGameplayObjects() const;
    
private:
    struct Square
    {
        enum class State
        {
            FREE,
            OBSTACLE,
            ENEMY
        };
        
        int x;
        int y;
        State state;
        Square()
        : x(0)
        , y(0)
        , state(State::FREE)
        {}
    };
    
    typedef std::vector<Square> Grid;
    
private:
    void SpawnObjects(const std::function<void(int, int)> &spawn, int amount);
    bool IsValidRow(int row) const;
    
    Grid _grid;
    GameplayObjects _objects;
    
    int _countObstacles;
    int _countEnemies;
    int _squaresByHeight;
};

#endif /* defined(__SlashingTrough__PathSector__) */
