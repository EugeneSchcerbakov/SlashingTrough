//
//  PopupCore.hpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 05/10/15.
//
//

#ifndef PopupCore_hpp
#define PopupCore_hpp

#include "cocos2d.h"
#include "ui/CocosGUI.h"

#include <queue>

class Popup : public cocos2d::ui::Layout
{
public:
    virtual void showEffect() = 0;
    virtual float hideEffect() = 0; // return effect duration
    
    virtual void setClose(bool close);
    
    virtual bool isClosing() const;
    
protected:
    Popup();
    virtual ~Popup();
    
    virtual bool init() override;
    
private:
    cocos2d::EventListenerTouchOneByOne *_inputLocker;
    
    bool _closing;
};

class PopupCarrier : public cocos2d::Scene
{
public:
    static PopupCarrier* create();
    
    void attachHandlerWithZOrder(int zOrder);
    
    void pushPopup(Popup *popup, int zOrder, std::string name);
    
protected:
    PopupCarrier();
    virtual ~PopupCarrier();
    
    virtual bool init() override;
    virtual void update(float dt) override;
    
    virtual void removePopup(Popup *popup);
    
private:
    bool touchBegan(cocos2d::Touch *touch, cocos2d::Event *e);
    
    std::queue<Popup *> _activePopups;
};

#endif /* PopupCore_hpp */
