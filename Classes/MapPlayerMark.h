//
//  MapPlayerMark.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 30/08/15.
//
//

#ifndef __SlashingTrough__MapPlayerMark__
#define __SlashingTrough__MapPlayerMark__

#include "cocos2d.h"
#include "ui/CocosGUI.h"

#include "FieldLevel.h"

class MapPlayerMark : public cocos2d::ui::Widget
{
public:
    static MapPlayerMark* create();
    
    void refreshPosition();
    
protected:
    MapPlayerMark();
    virtual ~MapPlayerMark();
    
    bool init();
    
private:
    FieldLevel::WeakPtr _currentLevel; // last completed
    cocos2d::Sprite *_mark;
};

#endif /* defined(__SlashingTrough__MapPlayerMark__) */
