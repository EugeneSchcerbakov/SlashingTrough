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

class PathSectorWidget : public cocos2d::Node
{
public:
    static PathSectorWidget* create(PathSector::WeakPtr path);
    
    void DrawDebugGrid();
    void ClearDebugGrid();
    
    cocos2d::Size GetSectorSize() const;
    
    PathSector::Ptr GetPath() const;
    
protected:
    PathSectorWidget(PathSector::WeakPtr path);
    virtual ~PathSectorWidget();
    
    bool init();
    
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
    
    PathSector::WeakPtr _path;
    
    cocos2d::DrawNode *_ground;
    cocos2d::DrawNode *_debugGrid;
};

#endif /* defined(__SlashingTrough__PathSectorWidget__) */
