//
//  StoreInterface.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 04/06/15.
//
//

#ifndef __SlashingTrough__StoreInterface__
#define __SlashingTrough__StoreInterface__

#include "StoreWeaponWidget.h"
#include "StoreCategoryButton.h"

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
        BACKING = 0,
        SCROLLER,
        DECORATION,
        CONTROLS
    };
    
    enum Category
    {
        WEAPON = 0,
        ARMOR,
        BOOSTERS,
        AMOUNT
    };
    
private:
    void OnBackPressed(cocos2d::Ref *sender, cocos2d::ui::Widget::TouchEventType event);
    void OnCategoryChanged(Category tab);
    void FillScrollerWithWeapons();
    void FillScrollerWithArmors();
    void FillScrollerWithBoosters();
    
    std::string _prevSceneName;
    std::vector<CategoryButton *> _tabButtons;

    cocos2d::ui::Text *_coinsText;
    cocos2d::ui::Text *_damageText;
    cocos2d::ui::Button *_coinsShopBtn;
    cocos2d::ui::Button *_backBtn;
    cocos2d::ui::ListView *_scroller;
};

#endif /* defined(__SlashingTrough__StoreInterface__) */
