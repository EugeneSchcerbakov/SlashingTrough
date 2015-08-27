//
//  MapInterface.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 23/08/15.
//
//

#ifndef __SlashingTrough__MapInterface__
#define __SlashingTrough__MapInterface__

#include "MapWidget.h"

class MapInterface : public cocos2d::Scene
{
public:
    static MapInterface* create();
    
protected:
    MapInterface();
    virtual ~MapInterface();
    
    bool init();
    
private:
    cocos2d::LayerColor *_background;
    cocos2d::Layer *_guiLayer;
    MapWidget *_mapWidget;
    
    cocos2d::ui::Text *_coinsText;
    cocos2d::ui::Text *_damageText;
};

#endif /* defined(__SlashingTrough__MapInterface__) */
