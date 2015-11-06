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

class GameInterface : public cocos2d::Layer
{
public:
    static GameInterface* create();
    
    void setTimeScaleLabel(float timeScale);
    void setDifficultLable(const std::string &text);
    
    void setGoldPointsLabel(int value);
    void setKillPointsLabel(int value);
    void setDamagePointsLabel(int value);
    void setHealthPointsLabel(float value);
    
protected:
    GameInterface();
    virtual ~GameInterface();
    
    bool init();
    void update(float dt);
    
private:
    cocos2d::FiniteTimeAction* cretePulsationWithColor() const;
    
    cocos2d::ProgressTimer *_hpBar;
    cocos2d::Sprite *_hpBack;
    cocos2d::Sprite *_coinsIcon;
    cocos2d::ui::Text *_goldPointsText;
    
    // debug output
    cocos2d::ui::Text *_timeScaleText;
    cocos2d::ui::Text *_difficultText;
};

#endif /* defined(__SlashingTrough__GameInterface__) */
