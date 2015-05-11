//
//  StaminaBarWidget.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 11/05/15.
//
//

#ifndef __SlashingTrough__StaminaBarWidget__
#define __SlashingTrough__StaminaBarWidget__

#include "cocos2d.h"

class StaminaBarWidget : public cocos2d::Node
{
public:
    static StaminaBarWidget* create();
    
    void setProgress(float progress);
    
protected:
    StaminaBarWidget();
    virtual ~StaminaBarWidget();
    
    bool init();
    void update(float dt);
    
private:
    cocos2d::Sprite *_bar;
    cocos2d::Sprite *_back;
    
    float _logicalProgress;
    float _visibleProgress;
};

#endif /* defined(__SlashingTrough__StaminaBarWidget__) */
