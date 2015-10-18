//
//  PauseMenu.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 10/09/15.
//
//

#include "PauseMenu.h"

#include "DailyMissions.h"
#include "ScreenChanger.h"
#include "Log.h"

PauseMenu* PauseMenu::create()
{
    PauseMenu *pauseMenu = new PauseMenu();
    if (pauseMenu && pauseMenu->init()) {
        pauseMenu->autorelease();
    } else {
        delete pauseMenu;
        pauseMenu = nullptr;
    }
    return pauseMenu;
}

PauseMenu::PauseMenu()
: _isGamePaused(false)
{
}

PauseMenu::~PauseMenu()
{
}

bool PauseMenu::hitTest(const cocos2d::Vec2 &pt)
{
    bool panelTest = false;
    auto panel = getChildByName<cocos2d::ui::Layout *>("panel");
    if (panel) {
        panelTest = panel->hitTest(pt);
    }
    return _pauseButton->hitTest(pt) || panelTest;
}

bool PauseMenu::init()
{
    if (!cocos2d::Layer::init()) {
        return false;
    }
    
    auto director = cocos2d::Director::getInstance();
    
    cocos2d::Vec2 origin = director->getVisibleOrigin();
    cocos2d::Size size = director->getVisibleSize();
    
    auto onPausePressed = [this](cocos2d::Ref *ref, cocos2d::ui::Widget::TouchEventType e)
    {
        if (e == cocos2d::ui::Widget::TouchEventType::ENDED) {
            if (_isGamePaused) {
                sendMessageAboutResume();
                hidePanel();
            } else {
                sendMessageAboutPause();
                showPanel();
            }
            _isGamePaused = !_isGamePaused;
        }
    };
    
    _pauseButton = cocos2d::ui::Button::create("ui/ui_btn_pause.png");
    _pauseButton->setScale(2.2f);
    _pauseButton->setPositionX(origin.x + _pauseButton->getContentSize().width * 2.2f * 0.5f);
    _pauseButton->setPositionY(origin.y + size.height - _pauseButton->getContentSize().height * 2.2f * 0.5f);
    _pauseButton->addTouchEventListener(onPausePressed);
    
    _tint = cocos2d::Sprite::create("ui/ui_bg_tint.png");
    _tint->setScale(4.0f);
    _tint->setVisible(false);
    
    addChild(_tint, Order::Tint);
    addChild(_pauseButton, Order::Button);
    
    return true;
}

void PauseMenu::showPanel()
{
    if (getChildByName("panel")) {
        WRITE_LOG("Trying to show paus pause panel then another one is already shown.");
        return;
    }
    
    auto director = cocos2d::Director::getInstance();
    auto dispatcher = director->getEventDispatcher();
    
    cocos2d::Vec2 origin = director->getVisibleOrigin();
    cocos2d::Size size = director->getVisibleSize();
    
    auto touchBegan = [this](cocos2d::Touch *touch, cocos2d::Event *e)
    {
        if (_isGamePaused)
        {
            cocos2d::Vec2 pt = convertTouchToNodeSpace(touch);
            if (!hitTest(pt)) {
                sendMessageAboutResume();
                hidePanel();
                _isGamePaused = false;
                return false;
            }
        }
        return true;
    };
    
    auto input = cocos2d::EventListenerTouchOneByOne::create();
    input->onTouchBegan = touchBegan;
    dispatcher->addEventListenerWithSceneGraphPriority(input, this);
    
    auto panel = cocos2d::ui::Layout::create();
    panel->setBackGroundImage("ui/ui_pause_menu_panel.png");
    panel->setContentSize(panel->getBackGroundImageTextureSize());
    
    auto onMapPressed = [this](cocos2d::Ref *ref, cocos2d::ui::Widget::TouchEventType e)
    {
        if (e == cocos2d::ui::Widget::TouchEventType::ENDED)
        {
            ScreenChanger::changeScreen(ScreenChanger::MAP);
        }
    };
    
    auto onAudioPressed = [this](cocos2d::Ref *ref, cocos2d::ui::Widget::TouchEventType e)
    {
    };
    
    auto onRestartPressed = [this](cocos2d::Ref *ref, cocos2d::ui::Widget::TouchEventType e)
    {
    };
    
    auto btnMap = cocos2d::ui::Button::create("ui/ui_btn_map.png");
    float btnScale = 1.8f;
    btnMap->setScale(btnScale);
    btnMap->setPositionX(btnMap->getContentSize().width * btnScale * 0.5f);
    btnMap->setPositionY(320.0f);
    btnMap->addTouchEventListener(onMapPressed);

    auto btnAudio = cocos2d::ui::Button::create("ui/ui_btn_audio.png");
    btnAudio->setScale(btnScale);
    btnAudio->setPositionX(btnAudio->getContentSize().width * btnScale * 0.5f);
    btnAudio->setPositionY(195.0f);
    btnAudio->addTouchEventListener(onAudioPressed);
    
    auto btnRestart = cocos2d::ui::Button::create("ui/ui_btn_restart.png");
    btnRestart->setScale(btnScale);
    btnRestart->setPositionX(btnRestart->getContentSize().width * btnScale * 0.5f);
    btnRestart->setPositionY(70.0f);
    btnRestart->addTouchEventListener(onRestartPressed);
    
    panel->addChild(btnMap);
    panel->addChild(btnAudio);
    panel->addChild(btnRestart);
    
    float x0 = 0.0f;
    float y0 = origin.y + size.height;
    float y1 = origin.y + size.height - panel->getContentSize().height;
    auto move = cocos2d::MoveTo::create(0.5f, cocos2d::Vec2(x0, y1));
    auto show = cocos2d::EaseBackOut::create(move);
    panel->setPositionX(x0);
    panel->setPositionY(y0);
    panel->runAction(show);
    
    auto fadein = cocos2d::FadeIn::create(0.3f);
    _tint->setVisible(true);
    _tint->setOpacity(0);
    _tint->runAction(fadein);
    
    addChild(panel, Order::Panel, "panel");
}

void PauseMenu::hidePanel()
{
    auto panel = getChildByName<cocos2d::ui::Layout *>("panel");
    
    if (panel)
    {
        auto director = cocos2d::Director::getInstance();
        auto dispatcher = director->getEventDispatcher();
        
        dispatcher->removeEventListenersForTarget(this);
        
        cocos2d::Vec2 origin = director->getVisibleOrigin();
        cocos2d::Size size = director->getVisibleSize();
        
        float x0 = 0.0f;
        float y0 = origin.y + size.height;
        
        auto funcClear = [&]()
        {
            removeChildByName("panel");
        };
        
        auto move = cocos2d::MoveTo::create(0.5f, cocos2d::Vec2(x0, y0));
        auto hide = cocos2d::EaseBackInOut::create(move);
        auto call = cocos2d::CallFunc::create(funcClear);
        auto eff = cocos2d::Sequence::create(hide, call, nullptr);
        panel->runAction(eff);
        
        auto tinthide = cocos2d::CallFunc::create([&](){_tint->setVisible(false);});
        auto tintfade = cocos2d::FadeOut::create(0.3f);
        auto tinteff = cocos2d::Sequence::create(tintfade, tinthide, nullptr);
        _tint->runAction(tinteff);
    }
}

void PauseMenu::sendMessageAboutPause()
{
    auto director = cocos2d::Director::getInstance();
    auto dispatcher = director->getEventDispatcher();
    dispatcher->dispatchCustomEvent("PauseGame");
}

void PauseMenu::sendMessageAboutResume()
{
    auto director = cocos2d::Director::getInstance();
    auto dispatcher = director->getEventDispatcher();
    dispatcher->dispatchCustomEvent("ResumeGame");
}
