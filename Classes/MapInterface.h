//
//  MapInterface.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 23/08/15.
//
//

#ifndef __SlashingTrough__MapInterface__
#define __SlashingTrough__MapInterface__

#include "MissionStartPopup.h"
#include "MapWidget.h"

class MapInterface : public PopupCarrier
{
public:
    static MapInterface* create();
    
protected:
    MapInterface();
    virtual ~MapInterface();
    
    bool init();
    
private:
    bool mapTouchBegan(cocos2d::Touch *touch, cocos2d::Event *e);
    void mapTouchEnded(cocos2d::Touch *touch, cocos2d::Event *e);
    void mapTouchCanceled(cocos2d::Touch *touch, cocos2d::Event *e);
    
    enum Order
    {
        COLOR = 0,
        MAP,
        POPUP,
        CONTROLS
    };
    
    cocos2d::LayerColor *_background;
    cocos2d::Layer *_guiLayer;
    MapWidget *_mapWidget;
    
    cocos2d::ui::Text *_coinsText;
};

#endif /* defined(__SlashingTrough__MapInterface__) */
