//
//  StoreCategoryButton.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 06/06/15.
//
//

#include "StoreCategoryButton.h"

CategoryButton* CategoryButton::create(const std::string &normal, const std::string &pressed,
                                       const std::function<void(int)> &func)
{
    CategoryButton *button = new CategoryButton(normal, pressed, func);
    if (button && button->init()) {
        button->autorelease();
    } else {
        delete button;
        button = nullptr;
    }
    return button;
}
    
bool CategoryButton::isSelected() const
{
    return _selected;
}
    
void CategoryButton::setSelected(bool flag)
{
    _selected = flag;
    std::string texture = _selected ? _selectTex : _normalTex;
    _backing->setTexture(texture);
    setContentSize(_backing->getContentSize() * _normalScale);
    if (_selected) {
        _callback(getTag());
    }
}

CategoryButton::CategoryButton(const std::string &normal, const std::string &pressed,
                               const std::function<void(int)> &func)
: _normalTex(normal)
, _selectTex(pressed)
, _normalScale(1.8f)
, _pressedScale(1.9f)
, _selected(false)
, _callback(func)
{
}

CategoryButton::~CategoryButton()
{
}
    
bool CategoryButton::init()
{
    if (!cocos2d::Node::init()) {
        return false;
    }
        
    _backing = cocos2d::Sprite::create();
    _backing->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE_BOTTOM);
        
    cocos2d::EventListenerTouchOneByOne *listener;
    listener = cocos2d::EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(CategoryButton::onTouchBegan, this);
    listener->onTouchEnded = CC_CALLBACK_2(CategoryButton::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(CategoryButton::onTouchCancelled, this);
        
    auto director = cocos2d::Director::getInstance();
    auto dispatcher = director->getEventDispatcher();
    dispatcher->addEventListenerWithSceneGraphPriority(listener, this);
        
    setSelected(_selected);
    setScale(_normalScale);
    addChild(_backing, 0);
        
    return true;
}
    
bool CategoryButton::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    cocos2d::Vec2 worldPos = touch->getLocation();
    cocos2d::Vec2 localPos = convertToNodeSpace(worldPos);
    if (_backing->getBoundingBox().containsPoint(localPos))
    {
        if (!_selected)
            setScale(_pressedScale);
        return true;
    }
    return false;
}
    
void CategoryButton::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    cocos2d::Vec2 worldPos = touch->getLocation();
    cocos2d::Vec2 localPos = convertToNodeSpace(worldPos);
    if (_backing->getBoundingBox().containsPoint(localPos))
    {
        if (!_selected) {
            _selected = true;
            setScale(_normalScale);
            setSelected(_selected);
        }
    }
}
    
void CategoryButton::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
    setScale(_normalScale);
    _backing->setTexture(_selectTex);
}
