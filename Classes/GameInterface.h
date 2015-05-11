//
//  GameInterface.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 11/04/15.
//
//

#ifndef __SlashingTrough__GameInterface__
#define __SlashingTrough__GameInterface__

#include "cocos2d.h"
#include "ui/CocosGUI.h"

#include "StaminaBarWidget.h"

class GameInterface : public cocos2d::Layer
{
public:
    static GameInterface* create();
    
    void SetTimeScaleLabel(float timeScale);
    void SetGoldPointsLabel(int value);
    void SetKillPointsLabel(int value);
    void SetDamagePointsLabel(int value);
    void SetHealthPointsLabel(int value);
    void SetStaminaPoints(float value);
    
protected:
    GameInterface();
    virtual ~GameInterface();
    
    bool init();
    
private:
    cocos2d::ui::Text *_goldPointsText;
    cocos2d::ui::Text *_killPointsText;
    cocos2d::ui::Text *_damagePointsText;
    cocos2d::ui::Text *_healthPointsText;
    cocos2d::ui::Text *_timeScaleText;
    
    StaminaBarWidget *_staminaBar;
};

#endif /* defined(__SlashingTrough__GameInterface__) */
