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

struct Square
{
    int x;
    int y;
    Entity::Type type;
    std::string name;
    
    Square()
    : x(-1)
    , y(-1)
    , type(Entity::Type::NONE)
    {
    }
};

struct Preset
{
    std::vector<Square> map;
    std::string id;
    int difficult;
    int squaresByWidth;
    int squaresByHeight;
    
    Preset()
    : difficult(0)
    , squaresByWidth(0)
    , squaresByHeight(0)
    {
    }
};

class PresetsLoader
{
public:
    static PresetsLoader& getInstance();
    
    void load(const std::string &filename);
    
    Preset getPresetById(const std::string &id) const;
    Preset getRandomPresetWithDif(int difficut) const;
    
    Preset getLinkById(const std::string &id) const;
    Preset getRandomLink() const;
    
private:
    static const std::string EMPTY_SQUARE;
    
private:
    int calcPresetWidth(tinyxml2::XMLElement *elem) const;
    int calcPresetHeight(tinyxml2::XMLElement *elem) const;
    
    std::multimap<int, Preset> _presets;
    std::vector<Preset> _links;
};

class FieldSector : public ModelBase
{
public:
    typedef std::shared_ptr<FieldSector> Ptr;
    typedef std::weak_ptr<FieldSector> WeakPtr;
    
    static FieldSector::Ptr create();
    
public:
    FieldSector();
    
    void init(const Preset &preset, float runningSpeed);
    
    void setX(float x);
    void setY(float y);
    
    float getWidth() const;
    float getHeight() const;
    float getX() const;
    float getY() const;
    float getRunningSpeed() const;
    int getSquaresNumByX() const;
    int getSquaresNumByY() const;
    
    const std::vector<Square>& getContent() const;
    const std::string& getPresetId() const;
    
private:
    std::vector<Square> _content;
    std::string _presetId;
    
    float _width;
    float _height;
    float _logicX; // bottom left corner
    float _logicY;
    float _runningSpeed;
    
    int _squareSize;
    int _squaresByWidth;
    int _squaresByHeight;
};

#endif /* defined(__SlashingTrough__FieldSector__) */
