//
//  CheatsPanel.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 20/12/15.
//
//

#ifndef CheatsPanel_h
#define CheatsPanel_h

#include "cocos2d.h"

class CheatsPanel : public cocos2d::Node
{
public:
    static CheatsPanel* create();
    
protected:
    CheatsPanel();
    virtual ~CheatsPanel();
    
    virtual bool init() override;
    
    void createPanel();
    void destroyPanel();
    bool panelOnScreen() const;
    
private:
    bool touchBegan(cocos2d::Touch *touch, cocos2d::Event *e);
    void touchEnded(cocos2d::Touch *touch, cocos2d::Event *e);
    void touchCanceled(cocos2d::Touch *touch, cocos2d::Event *e);
    
    cocos2d::Vec2 _touchDown;
    cocos2d::Rect _tapArea;
    cocos2d::Rect _panelRect;
    
    static const std::string panelName;
};

#endif /* CheatsPanel_h */
