//
//  StatisticsInterface.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 05/09/15.
//
//

#ifndef __SlashingTrough__StatisticsInterface__
#define __SlashingTrough__StatisticsInterface__

#include "cocos2d.h"
#include "ui/CocosGUI.h"

#include "FieldLevel.h"
#include "PlayerInfo.h"

class StaticticsInterface : public cocos2d::Scene
{
public:
    static StaticticsInterface* create(FieldLevel::WeakPtr level, PlayerInfo::Score score, bool victory);

protected:
    StaticticsInterface();
    virtual ~StaticticsInterface();
    
    bool init(FieldLevel::WeakPtr level, PlayerInfo::Score score, bool victory);
    
private:
    bool onTouch(cocos2d::Touch *touch, cocos2d::Event *e);
};

#endif /* defined(__SlashingTrough__StatisticsInterface__) */
