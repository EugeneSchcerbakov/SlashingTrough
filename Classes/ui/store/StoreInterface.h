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
#include "StoreArmorWidget.h"
#include "StoreCrystallWidget.h"
#include "StoreCategoryButton.h"

class StoreInterface : public cocos2d::Scene
{
public:
    static StoreInterface* create();
    
protected:
    StoreInterface();
    virtual ~StoreInterface();
    
    bool init();
    void update(float dt);
    
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
        CRYSTALL,
        AMOUNT
    };
    
private:
    void onBackPressed(cocos2d::Ref *sender, cocos2d::ui::Widget::TouchEventType event);
    void onCategoryChanged(Category tab);
    void fillScrollerWithWeapons();
    void fillScrollerWithArmors();
    void fillScrollerWithCrystalls();
    
    std::vector<CategoryButton *> _tabButtons;

    cocos2d::ui::Text *_coinsText;
    cocos2d::ui::Button *_coinsShopBtn;
    cocos2d::ui::Button *_backBtn;
    cocos2d::ui::ListView *_scroller;
};

#endif /* defined(__SlashingTrough__StoreInterface__) */
