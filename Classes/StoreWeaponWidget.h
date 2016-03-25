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
#include "StoreItemBaseWidget.h"

class StoreWeaponWidget : public StoreItemBaseWidget
{
public:
    static StoreWeaponWidget* create(Item::WeakPtr item);
    
protected:
    StoreWeaponWidget(Item::WeakPtr item);
    virtual ~StoreWeaponWidget();
    
    bool init();

    virtual void onBuy() override;
    
private:
    static std::string chooseSpeedLabel(float speed);
};

#endif /* defined(__SlashingTrough__StoreItemWidget__) */
