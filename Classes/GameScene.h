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
    bool init() override;
    
private:
    enum LayerZOrder
    {
        EFFECTS_ABSOLUTE,
        GAME_INTERFACE,
        EFFECTS_GAMEFIELD,
        GAME_FIELD
    };
    
private:
    GameField *_layerField;
};

#endif /* defined(__SlashingTrough__GameScene__) */
