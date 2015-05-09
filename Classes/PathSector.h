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
    
    void Generate(int squaresByHeight);
    void SpawnObjects(const GameInfo::DifficultInfo::SpawnList &obstacles,
                      const GameInfo::DifficultInfo::SpawnList &enemies);
    void Reset();
    
    Square GetSquareByLocalXY(float x, float y) const;
    Square GetSquareByIndexXY(int x, int y) const;
    Square GetSquareByObject(GameplayObject::Ptr object) const;
    
    int GetSquaresByHeight() const;
    
    GameplayObject::WeakPtr GetObjectByUID(int uid);
    GameplayObjects& GetGameplayObjects();
    
    bool IsValidSquareAddress(int x, int y) const;
    
private:
    typedef std::vector<Square> Grid;
    
private:
    void GenerateBunchOfObjects(const std::function<void(int, int, const std::string &)> &spawn,
                      const GameInfo::SpawnInfo &spawnInfo);
    bool IsValidRow(int row) const;
    GameplayObject::UID GenerateUID();
    
    Grid _grid;
    GameplayObjects _objects;
    
    const float _squareSize;
    
    int _squaresByHeight;
};

#endif /* defined(__SlashingTrough__PathSector__) */
