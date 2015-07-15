//
//  FieldSector.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 11/07/15.
//
//

#ifndef __SlashingTrough__FieldSector__
#define __SlashingTrough__FieldSector__

#include <stdio.h>
#include <memory>
#include <vector>
#include <functional>

#include "ModelBase.h"
#include "GameInfo.h"

class Field;

struct Square
{
    int x;
    int y;
    bool free;
    Square()
    : x(-1)
    , y(-1)
    , free(true)
    {
    }
};

class FieldSector : public ModelBase
{
public:
    typedef std::shared_ptr<FieldSector> Ptr;
    typedef std::weak_ptr<FieldSector> WeakPtr;
    
    static FieldSector::Ptr create();
    
public:
    FieldSector();
    
    Entities generate(int squaresByHeight, const GameInfo::DifficultInfo difficult, Field *field);
    
    void reset();
    void setX(float x);
    void setY(float y);
    void setSquareFree(int x, int y, bool flag);
    
    Square getSquareByLogicXY(float x, float y) const;
    Square getSquareByIndexXY(int x, int y) const;
    
    float getWidth() const;
    float getHeight() const;
    float getX() const;
    float getY() const;
    int getSquaresNumByX() const;
    int getSquaresNumByY() const;
    
    bool isValidSquareAddress(int x, int y) const;
    bool isValidRow(int row) const;
    
private:
    struct ConstructionInfo {
        Square square;
        Entity::Type type;
        std::string name;
    };
    
    typedef std::vector<Square> Grid;
    typedef std::vector<ConstructionInfo> ConstructionMap;
    
private:
    void generateConstructionMap(ConstructionMap &map, const GameInfo::SpawnInfo &spawn, Entity::Type type);
    
    Grid _grid;
    
    float _width;
    float _height;
    float _logicX; // bottom left corner
    float _logicY;
    
    int _squareSize;
    int _squaresByWidth;
    int _squaresByHeight;
};

#endif /* defined(__SlashingTrough__FieldSector__) */
