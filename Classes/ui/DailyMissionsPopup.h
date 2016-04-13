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
#include "PopupCore.h"

class DailyMissionPopup : public Popup
{
public:
    static DailyMissionPopup* create(cocos2d::Layer *container);
    
    bool hitTest(const cocos2d::Vec2 &pt, const cocos2d::Camera* camera, cocos2d::Vec3 *p) const override;
    
    void addTaskRewardEffect(const std::string &taskId, cocos2d::Vec2 endp);
    
    virtual void showEffect() override;
    virtual float hideEffect() override;
    
protected:
    DailyMissionPopup(cocos2d::Layer *container);
    virtual ~DailyMissionPopup();
    
    virtual bool init() override;
    virtual void update(float dt) override;
    
private:
    void checkInternalState();
    
    cocos2d::Sprite *_panel;
    cocos2d::Layer *_globalEffects;
    
    std::list<std::pair<std::string, cocos2d::Vec2>> _effects;
    
    bool _shown;
};

#endif /* DailyMissionsPopup_hpp */
