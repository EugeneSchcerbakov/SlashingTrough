//
//  DailyMissionsPopup.hpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 11/10/15.
//
//

#ifndef DailyMissionsPopup_hpp
#define DailyMissionsPopup_hpp

#include "DailyMissions.h"
#include "EffectsLayer.h"
#include "PopupCore.h"

class DailyMissionPopup : public Popup
{
public:
    static DailyMissionPopup* create(EffectsLayer *container);
    
    bool hitTest(const cocos2d::Vec2 &pt) override;
    
    void addTaskRewardEffect(const std::string &taskId, cocos2d::Vec2 endp);
    
    virtual void showEffect() override;
    virtual float hideEffect() override;
    
protected:
    DailyMissionPopup(EffectsLayer *container);
    virtual ~DailyMissionPopup();
    
    virtual bool init() override;
    virtual void update(float dt) override;
    
private:
    void checkInternalState();
    
    cocos2d::Sprite *_panel;
    EffectsLayer *_globalEffects;
    
    std::list<std::pair<std::string, cocos2d::Vec2>> _effects;
    
    bool _shown;
};

#endif /* DailyMissionsPopup_hpp */
