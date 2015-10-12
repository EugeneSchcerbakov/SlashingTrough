//
//  StoreCrystallWidget.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 18/09/15.
//
//

#ifndef __SlashingTrough__StoreCrystallWidget__
#define __SlashingTrough__StoreCrystallWidget__

#include "Store.h"
#include "StoreItemBaseWidget.h"

class StoreCrystallWidget : public StoreItemBaseWidget
{
public:
    static StoreCrystallWidget* create(Item::WeakPtr item);
    
protected:
    StoreCrystallWidget(Item::WeakPtr item);
    virtual ~StoreCrystallWidget();
    
    bool init() override;
    void update(float dt) override;
    
    void onBuyPressed(cocos2d::Ref *sender, cocos2d::ui::Widget::TouchEventType event) override;
    
private:
    void initShardsPanel();
};

#endif /* defined(__SlashingTrough__StoreCrystallWidget__) */
