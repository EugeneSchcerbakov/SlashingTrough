//
//  GameScene.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 04/04/15.
//
//

#include "GameScene.h"

#include "GameInfo.h"
#include "Utils.h"

GameScene* GameScene::create(const std::string &levelId)
{
    GameScene *gamescene = new GameScene();
    if (gamescene && gamescene->init(levelId)) {
        gamescene->autorelease();
    } else {
        delete gamescene;
        gamescene = nullptr;
    }
    return gamescene;
}

GameScene::GameScene()
: _mouseListener(nullptr)
, _isTimeScaleAllowed(false)
, _currentTimeScale(1.0f)
{
}

GameScene::~GameScene()
{
}

bool GameScene::init(const std::string &levelId)
{
    if (!cocos2d::Scene::init()) {
        return false;
    }

    auto director = cocos2d::Director::getInstance();
    auto dispatcher = director->getEventDispatcher();
    
    if (misc::isPlatformDesctop())
    {
        _mouseListener = cocos2d::EventListenerMouse::create();
        _mouseListener->onMouseScroll = [&](cocos2d::Event *event){OnMouseScroll(event);};
        dispatcher->addEventListenerWithSceneGraphPriority(_mouseListener, this);
        
        _keyboardListener = cocos2d::EventListenerKeyboard::create();
        _keyboardListener->onKeyPressed = CC_CALLBACK_2(GameScene::OnKeyPressed, this);
        _keyboardListener->onKeyReleased = CC_CALLBACK_2(GameScene::OnKeyReleased, this);
        dispatcher->addEventListenerWithSceneGraphPriority(_keyboardListener, this);
    }
    
    _layerGui = GameInterface::create();
    _layerField = FieldLayer::create(levelId, _layerGui);
    
    addChild(_layerField, LayerZOrder::GAME_FIELD);
    addChild(_layerGui, LayerZOrder::GAME_INTERFACE);
    
    return true;
}

void GameScene::OnKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event)
{
    if (keyCode == cocos2d::EventKeyboard::KeyCode::KEY_CTRL) {
        _isTimeScaleAllowed = true;
    }
}

void GameScene::OnKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event)
{
    if (keyCode == cocos2d::EventKeyboard::KeyCode::KEY_CTRL) {
        _isTimeScaleAllowed = false;
    }
}

void GameScene::OnMouseScroll(cocos2d::Event *event)
{
    if (event->getType() != cocos2d::Event::Type::MOUSE) {
        return;
    }
    
    if (_isTimeScaleAllowed)
    {
        cocos2d::EventMouse *mouseEvent;
        mouseEvent = dynamic_cast<cocos2d::EventMouse *>(event);
        const float max = 20.0f;
        const float min = 0.1f;
        const bool dir = mouseEvent->getScrollY() > 0.0f;
        
        if (dir && _currentTimeScale * 2.0f < max) {
            _currentTimeScale = _currentTimeScale * 2.0f;
        }
        if (!dir && _currentTimeScale / 2.0f > min) {
            _currentTimeScale = _currentTimeScale / 2.0f;
        }
        
        cocos2d::Director *director = cocos2d::Director::getInstance();
        cocos2d::Scheduler *scheduler = director->getScheduler();
        scheduler->setTimeScale(_currentTimeScale);
        _layerGui->setTimeScaleLabel(_currentTimeScale);
    }
}
