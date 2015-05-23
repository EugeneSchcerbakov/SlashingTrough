//
//  PathSectorWidget.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 05/04/15.
//
//

#ifndef __SlashingTrough__PathSectorWidget__
#define __SlashingTrough__PathSectorWidget__

#include "cocos2d.h"
#include "PathSector.h"
#include "GameplayObjectsWidgets.h"

class HeroWidget;

class PathSectorWidget : public cocos2d::Node
{
public:
    typedef std::list<PathSectorWidget *> SectorsSequence;
    typedef SectorsSequence::iterator SectorsSequenceIter;
    
public:
    static PathSectorWidget* create(PathSector::Ptr path, HeroWidget *hero);
    
    void DrawDebugGrid();
    void ClearDebugGrid();
    void SetSnapPos(const cocos2d::Vec2 &snapPos);
    
    cocos2d::Size GetSectorSize() const;
    cocos2d::Vec2 GetSnapPos() const;
    
    PathSector::Ptr GetPath() const;
    
protected:
    PathSectorWidget(PathSector::Ptr path, HeroWidget *hero);
    virtual ~PathSectorWidget();
    
    bool init();
    void update(float dt);
    
private:
    enum DrawOrder
    {
        GROUND,
        DEBUG_GREED,
        OBSTACLES,
        BONUSES,
        ENEMIES
    };
    
private:
    float _sectorWidth;
    float _sectorHeight;
    cocos2d::Vec2 _snapPos;
    
    std::map<GameplayObject::UID, cocos2d::Node *> _widgets;
    
    PathSector::Ptr _path;
    HeroWidget *_heroWidget;
    
    cocos2d::DrawNode *_ground;
    cocos2d::DrawNode *_debugGrid;
};

#endif /* defined(__SlashingTrough__PathSectorWidget__) */
