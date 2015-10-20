//
//  EffectFlyingIcon.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 18/10/15.
//
//

#ifndef EffectFlyingIcon_hpp
#define EffectFlyingIcon_hpp

#include "EffectsLayer.h"

class EffectFlyingIcon : public Effect
{
public:
    static EffectFlyingIcon* create(std::string icon, cocos2d::Vec2 from, cocos2d::Vec2 to, float startScale);
    
    virtual void setOpacity(GLubyte opacity) override;
    
protected:
    EffectFlyingIcon();
    virtual ~EffectFlyingIcon();
    
    bool init(std::string icon, cocos2d::Vec2 from, cocos2d::Vec2 to, float startScale);
    
private:
    cocos2d::Sprite *_sprite;
};

#endif /* EffectDailyTaskReward_hpp */
