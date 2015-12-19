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
#include "tinyxml2/tinyxml2.h"

#include "ModelBase.h"
#include "GameInfo.h"

class Field;

struct SpawnSquare
{
    int x, y;
    Entity::Type type;
    std::string group; // range, melli or stand
    std::string entityId; // will use il level generation stage
    
    SpawnSquare() = default;
};

struct Preset
{
    std::vector<SpawnSquare> spawnMap;
    std::string id;
    int enemiesAmount;
    int obsticlesAmount;
    int squaresByWidth;
    int squaresByHeight;
    
    Preset()
    : enemiesAmount(0)
    , obsticlesAmount(0)
    , squaresByWidth(0)
    , squaresByHeight(0)
    {}
};

class PresetsLibrary
{
public:
    static PresetsLibrary& getInstance();
    
    void load(const std::string &filename);
    
    Preset getRandomWithEnemiesAmount(int amount) const;
    Preset getPResetById(const std::string &id) const;
    
private:
    static const std::string EMPTY_SQUARE;
    
private:
    int calcPresetWidth(tinyxml2::XMLElement *elem) const;
    int calcPresetHeight(tinyxml2::XMLElement *elem) const;
    
    std::multimap<int, Preset> _presets; // key - spawn points amount
};

class FieldSector : public ModelBase
{
public:
    typedef std::shared_ptr<FieldSector> Ptr;
    typedef std::weak_ptr<FieldSector> WeakPtr;
    
    static FieldSector::Ptr create();
    
public:
    FieldSector();
    
    void init(const Preset &preset);
    
    void setX(float x);
    void setY(float y);
    
    float getWidth() const;
    float getHeight() const;
    float getX() const;
    float getY() const;
    int getSquaresNumByX() const;
    int getSquaresNumByY() const;
    
    const std::vector<SpawnSquare>& getContent() const;
    const std::string& getPresetId() const;
    
private:
    std::vector<SpawnSquare> _content;
    std::string _presetId;
    
    float _width;
    float _height;
    float _logicX; // bottom left corner
    float _logicY;
    
    int _squareSize;
    int _squaresByWidth;
    int _squaresByHeight;
};

#endif /* defined(__SlashingTrough__FieldSector__) */
