//
//  GameScene.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 04/04/15.
//
//

#ifndef __SlashingTrough__GameScene__
#define __SlashingTrough__GameScene__

#include "cocos2d.h"

#include "GameField.h"

class GameScene : public cocos2d::Scene
{
public:
    static GameScene* create();
    
protected:
    GameScene();
    virtual ~GameScene();
    
    bool init() override;
    
private:
    enum LayerZOrder
    {
        GAME_FIELD,
        EFFECTS_GAMEFIELD,
        GAME_INTERFACE,
        EFFECTS_ABSOLUTE
    };
    
private:
    GameField *_layerField;
    cocos2d::Layer *_layerGui;
    
    void OnKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event);
    void OnKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event);
    void OnMouseScroll(cocos2d::Event *event);
    
    bool _isTimeScaleAllowed;
    float _currentTimeScale;
    cocos2d::EventListenerMouse *_mouseListener;
    cocos2d::EventListenerKeyboard *_keyboardListener;
};

#endif /* defined(__SlashingTrough__GameScene__) */
