//
//  GameScene.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 04/04/15.
//
//

#include "GameScene.h"

#include "GameInfo.h"

GameScene* GameScene::create()
{
    GameScene *gamescene = new GameScene();
    if (gamescene && gamescene->init()) {
        gamescene->autorelease();
    } else {
        delete gamescene;
        gamescene = nullptr;
    }
    return gamescene;
}

bool GameScene::init()
{
    if (!cocos2d::Scene::init()) {
        return false;
    }

    _layerField = GameField::create();
    _layerField->setScale(1.0f);
    
    addChild(_layerField, LayerZOrder::GAME_FIELD);
    
    return true;
}
