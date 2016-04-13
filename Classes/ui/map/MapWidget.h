//
//  MapWidget.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 23/08/15.
//
//

#ifndef __SlashingTrough__MapWidget__
#define __SlashingTrough__MapWidget__

#include "MapLevelMark.h"
#include "MapPlayerMark.h"

class MapWidget : public cocos2d::ui::ScrollView
{
public:
    static MapWidget* create(const std::string &mapDesc);
    
    MapLevelMark* getLevelUnderPoint(cocos2d::Vec2 point);
    
protected:
    MapWidget();
    virtual ~MapWidget();
    
    bool init(const std::string &mapDesc);
    
private:
    bool initLevelMarkers(const std::string &mapFile);
    
    cocos2d::Sprite *_map;
    MapPlayerMark *_playerMark;
    
    std::vector<MapLevelMark *> _levelMarkers;
};

#endif /* defined(__SlashingTrough__MapWidget__) */
