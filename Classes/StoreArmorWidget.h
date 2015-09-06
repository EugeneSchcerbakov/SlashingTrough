//
//  StoreArmorWidget.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 02/08/15.
//
//

#ifndef __SlashingTrough__StoreArmorWidget__
#define __SlashingTrough__StoreArmorWidget__

#include "Store.h"
#include "StoreItemBaseWidget.h"

class StoreArmorWidget : public StoreItemBaseWidget
{
public:
    static StoreArmorWidget* create(Item::WeakPtr item);
    
protected:
    StoreArmorWidget(Item::WeakPtr item);
    virtual ~StoreArmorWidget();
    
    bool init();
};

#endif /* defined(__SlashingTrough__StoreArmorWidget__) */
