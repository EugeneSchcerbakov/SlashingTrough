//
//  StartInterface.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 23/08/15.
//
//

#ifndef __SlashingTrough__StartInterface__
#define __SlashingTrough__StartInterface__

#include "cocos2d.h"
#include "ui/CocosGUI.h"

class StartInterface : public cocos2d::Scene
{
public:
    static StartInterface* create();
    
protected:
    StartInterface();
    virtual ~StartInterface();
    
    bool init();
    
private:
    bool onTouch(cocos2d::Touch *touch, cocos2d::Event *e);
};

#endif /* defined(__SlashingTrough__StartInterface__) */
