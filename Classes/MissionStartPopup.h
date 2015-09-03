//
//  MissionStartPopup.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 01/09/15.
//
//

#ifndef __SlashingTrough__MissionStartPopup__
#define __SlashingTrough__MissionStartPopup__

#include "cocos2d.h"
#include "ui/CocosGUI.h"

#include "FieldLevel.h"

class MissionStartPopup : public cocos2d::ui::Layout
{
public:
    static MissionStartPopup* create(const std::string &levelId, const std::string &title);
    
    bool hitTest(const cocos2d::Vec2 &pt) override;
    
    void startShowEffect();
    void startHideEffect(std::function<void()> func);
    
protected:
    MissionStartPopup();
    virtual ~MissionStartPopup();
    
    bool init(const std::string &levelId, const std::string &title);
    
    bool onBoardTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);

private:
    cocos2d::Sprite *_panel;
    cocos2d::Sprite *_tint;
    cocos2d::ui::Button *_play;
    
    FieldLevel::Ptr _level;
};

#endif /* defined(__SlashingTrough__MissionStartPopup__) */
