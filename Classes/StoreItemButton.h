//
//  StoreItemButton.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 06/06/15.
//
//

#ifndef __SlashingTrough__StoreItemButton__
#define __SlashingTrough__StoreItemButton__

#include "cocos2d.h"
#include "ui/CocosGUI.h"

class StoreItemButton : public cocos2d::ui::Button
{
public:
    enum class State
    {
        BUY = 0,
        EQUIP,
        EQUIPPED
    };
    
    static StoreItemButton* create();
    
    void switchState(State state);
    void setPrice(int price);
    bool isState(State state) const;
    
protected:
    StoreItemButton();
    virtual ~StoreItemButton();
    
    bool init();
    
private:
    void InitBuyState();
    void InitEquippedState();
    
    State _state;
    int _price;
};

#endif /* defined(__SlashingTrough__StoreItemButton__) */
