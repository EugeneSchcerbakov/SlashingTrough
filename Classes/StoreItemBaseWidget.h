//
//  StoreItemBaseWidget.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 02/08/15.
//
//

#ifndef __SlashingTrough__StoreItemBaseWidget__
#define __SlashingTrough__StoreItemBaseWidget__

#include <stdio.h>

#include "Store.h"
#include "StoreItemButton.h"

class StoreItemBaseWidget : public cocos2d::ui::Layout
{
protected:
    StoreItemBaseWidget(Equip::WeakPtr item);
    virtual ~StoreItemBaseWidget();
    
    virtual bool init();
    virtual void update(float dt);
    
    virtual void onBuyPressed(cocos2d::Ref *sender, cocos2d::ui::Widget::TouchEventType event);
    
    Equip::WeakPtr _item;
    StoreItemButton *_button;
};

#endif /* defined(__SlashingTrough__StoreItemBaseWidget__) */
