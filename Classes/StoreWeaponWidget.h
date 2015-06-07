//
//  StoreWeaponWidget.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 06/06/15.
//
//

#ifndef __SlashingTrough__StoreItemWidget__
#define __SlashingTrough__StoreItemWidget__

#include "Store.h"
#include "StoreItemButton.h"

class StoreWeaponWidget : public cocos2d::ui::Layout
{
public:
    static StoreWeaponWidget* create(Equip::WeakPtr item);
    
protected:
    StoreWeaponWidget(Equip::WeakPtr item);
    virtual ~StoreWeaponWidget();
    
    bool init();
    
private:
    void OnBuyPressed(cocos2d::Ref *sender, cocos2d::ui::Widget::TouchEventType event);
    static std::string ChooseSpeedLabel(float speed);
    
    Equip::WeakPtr _item;
    StoreItemButton *_button;
};

#endif /* defined(__SlashingTrough__StoreItemWidget__) */
