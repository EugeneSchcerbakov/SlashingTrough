//
//  DailyMissionsPopup.hpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 11/10/15.
//
//

#ifndef DailyMissionsPopup_hpp
#define DailyMissionsPopup_hpp

#include "PopupCore.h"

class DailyMissionPopup : public Popup
{
public:
    static DailyMissionPopup* create();
    
    bool hitTest(const cocos2d::Vec2 &pt) override;
    
    virtual void showEffect() override;
    virtual float hideEffect() override;
    
protected:
    DailyMissionPopup();
    virtual ~DailyMissionPopup();
    
    bool init();
    
private:
    cocos2d::Sprite *_panel;
};

#endif /* DailyMissionsPopup_hpp */
