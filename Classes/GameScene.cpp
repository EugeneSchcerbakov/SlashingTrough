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
#include "Log.h"

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
    auto director = cocos2d::Director::getInstance();
    auto dispatcher = director->getEventDispatcher();
    
    dispatcher->removeCustomEventListeners("PauseGame");
    dispatcher->removeCustomEventListeners("ResumeGame");
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
    
    auto funcOnPause = [this](cocos2d::EventCustom *e)
    {
        pauseRecursive(_layerField);
        _layerGui->setVisible(false);
        WRITE_LOG("Game paused.");
    };
    
    auto funcOnResume = [this](cocos2d::EventCustom *e)
    {
        resumeRecursive(_layerField);
        _layerGui->setVisible(true);
        WRITE_LOG("Game resumed.");
    };
    
    dispatcher->addCustomEventListener("PauseGame", funcOnPause);
    dispatcher->addCustomEventListener("ResumeGame", funcOnResume);
    
    _layerGui = GameInterface::create();
    _layerField = FieldLayer::create(levelId, _layerGui);
    _layerPause = PauseMenu::create();
    
    addChild(_layerField, LayerZOrder::GAME_FIELD);
    addChild(_layerGui, LayerZOrder::GAME_INTERFACE);
    addChild(_layerPause, LayerZOrder::PAUSE_MENU);
    
    return true;
}

void GameScene::pauseRecursive(cocos2d::Node *node)
{
    node->pause();
    for (auto child : node->getChildren()) {
        pauseRecursive(child);
    }
}

void GameScene::resumeRecursive(cocos2d::Node *node)
{
    node->resume();
    for (auto child : node->getChildren()) {
        resumeRecursive(child);
    }
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

