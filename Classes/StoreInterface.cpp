//
//  StoreInterface.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 04/06/15.
//
//

#include "StoreInterface.h"

#include "Utils.h"

StoreInterface* StoreInterface::create(const std::string &prevSceneName)
{
    StoreInterface *object = new StoreInterface(prevSceneName);
    if (object && object->init()) {
        object->autorelease();
    } else {
        delete object;
        object = nullptr;
    }
    return object;
}

StoreInterface::StoreInterface(const std::string &prevSceneName)
: _prevSceneName(prevSceneName)
{
}

StoreInterface::~StoreInterface()
{
}

bool StoreInterface::init()
{
    if (!cocos2d::Scene::init()) {
        return false;
    }
    
    cocos2d::Color4B backingColor(255, 229, 153, 255);
    cocos2d::LayerColor *backging = cocos2d::LayerColor::create(backingColor);
    
    cocos2d::Size frameSize = cocos2d::Director::getInstance()->getVisibleSize();
    
    cocos2d::Sprite *decorTop = cocos2d::Sprite::create("ui/ui_shop_wood-frame_top.png");
    decorTop->setPositionX(frameSize.width * 0.5f);
    decorTop->setPositionY(frameSize.height - decorTop->getContentSize().height * 0.5f);
    
    cocos2d::Sprite *decorBtm = cocos2d::Sprite::create("ui/ui_shop_wood-frame_down.png");
    decorBtm->setPositionX(frameSize.width * 0.5f);
    decorBtm->setPositionY(decorBtm->getContentSize().height * 0.5f);
    
    float backBtnScale = 1.8f;
    _backBtn = cocos2d::ui::Button::create("ui/ui_btn_back.png");
    _backBtn->setScale(backBtnScale);
    _backBtn->setPositionX(_backBtn->getContentSize().width * backBtnScale * 0.5f);
    _backBtn->setPositionY(_backBtn->getContentSize().height * backBtnScale * 0.5f);
    _backBtn->addTouchEventListener(CC_CALLBACK_2(StoreInterface::OnBackPressed, this));
    
    addChild(backging, Order::BACKING);
    addChild(decorTop, Order::DECORATION);
    addChild(decorBtm, Order::DECORATION);
    addChild(_backBtn, Order::CONTROLS);
    
    return true;
}

void StoreInterface::OnBackPressed(cocos2d::Ref *sender, cocos2d::ui::Widget::TouchEventType event)
{
    if (event == cocos2d::ui::Widget::TouchEventType::ENDED)
    {
        cocos2d::Director *director = cocos2d::Director::getInstance();
        std::string startSceneName = Utils::LuaGetGlobalString("StartScreenSceneName");
        std::string resultSceneName = Utils::LuaGetGlobalString("ResultScreenSceneName");
        if (_prevSceneName == startSceneName) {
            auto scene = Utils::MakeSceneFromLua("CreateStartscreenScene");
            auto trans = cocos2d::TransitionSlideInL::create(0.3f, scene);
            director->replaceScene(trans);
        } else if (_prevSceneName == resultSceneName) {
            auto scene = Utils::MakeSceneFromLua("CreateResultScene");
            auto trans = cocos2d::TransitionSlideInL::create(0.3f, scene);
            director->replaceScene(trans);
        } else {
            CC_ASSERT(false);
        }
    }
}
