//
//  StoreInterface.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 04/06/15.
//
//

#ifndef __SlashingTrough__StoreInterface__
#define __SlashingTrough__StoreInterface__

#include "cocos2d.h"
#include "ui/CocosGUI.h"

#include "Store.h"

class StoreInterface : public cocos2d::Scene
{
public:
    static StoreInterface* create(const std::string &prevSceneName);
    
protected:
    StoreInterface(const std::string &prevSceneName);
    virtual ~StoreInterface();
    
    bool init();
    
private:
    enum Order
    {
        BACKING,
        SCROLLER,
        DECORATION,
        CONTROLS
    };
    
private:
    void OnBackPressed(cocos2d::Ref *sender, cocos2d::ui::Widget::TouchEventType event);
    
    std::string _prevSceneName;

    cocos2d::ui::Button *_backBtn;
};

#endif /* defined(__SlashingTrough__StoreInterface__) */
