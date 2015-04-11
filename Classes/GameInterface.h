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
    
    void SetTimeScaleLabel(float timeScale);
    
protected:
    GameInterface();
    virtual ~GameInterface();
    
    bool init();
    
private:
    cocos2d::ui::Text *_timeScaleText;
};

#endif /* defined(__SlashingTrough__GameInterface__) */
