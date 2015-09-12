//
//  PauseMenu.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 10/09/15.
//
//

#ifndef __SlashingTrough__PauseMenu__
#define __SlashingTrough__PauseMenu__

#include "cocos2d.h"
#include "ui/CocosGUI.h"

class PauseMenu : public cocos2d::Layer
{
public:
    static PauseMenu* create();
    
    bool hitTest(const cocos2d::Vec2 &pt);
    
protected:
    PauseMenu();
    virtual ~PauseMenu();
    
    bool init();
    
private:
    enum Order
    {
        Tint,
        Panel,
        Button
    };
    
    void showPanel();
    void hidePanel();
    
    void sendMessageAboutPause();
    void sendMessageAboutResume();
    
    bool _isGamePaused;
    
    cocos2d::ui::Button *_pauseButton;
    cocos2d::Sprite *_tint;
};

#endif /* defined(__SlashingTrough__PauseMenu__) */
