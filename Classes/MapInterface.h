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
#include "DailyMissionsPopup.h"
#include "MapWidget.h"
#include "EffectsLayer.h"

class MapInterface : public PopupCarrier
{
public:
    static MapInterface* create();
    
protected:
    MapInterface();
    virtual ~MapInterface();
    
    bool init();
    
private:
    void checkDailyMissionsCompletness();
    
    bool mapTouchBegan(cocos2d::Touch *touch, cocos2d::Event *e);
    void mapTouchEnded(cocos2d::Touch *touch, cocos2d::Event *e);
    void mapTouchCanceled(cocos2d::Touch *touch, cocos2d::Event *e);
    
    enum Order
    {
        COLOR = 0,
        MAP,
        POPUP,
        CONTROLS,
        EFFECTS
    };
    
    cocos2d::LayerColor *_background;
    cocos2d::Layer *_guiLayer;
    MapWidget *_mapWidget;
    EffectsLayer *_effectsLayer;
    
    cocos2d::ui::Text *_coinsText;
    cocos2d::ui::Button *_shopButton;
    cocos2d::ui::Button *_dailyButton;
};

#endif /* defined(__SlashingTrough__MapInterface__) */
