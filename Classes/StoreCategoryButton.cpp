//
//  StoreCategoryButton.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 06/06/15.
//
//

#include "StoreCategoryButton.h"

CategoryButton* CategoryButton::create(const std::string &icon, const std::function<void(int)> &func)
{
    CategoryButton *button = new CategoryButton(func);
    if (button && button->init(icon)) {
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
    if (_selected) {
        _callback(getTag());
    }
}

CategoryButton::CategoryButton(const std::function<void(int)> &func)
: _normalTex("ui/ui_shop_tab_category.png")
, _selectTex("ui/ui_shop_tab_category_selected.png")
, _normalScale(1.0f)
, _pressedScale(1.1f)
, _selected(false)
, _callback(func)
{
}

CategoryButton::~CategoryButton()
{
}
    
bool CategoryButton::init(const std::string &icon)
{
    if (!cocos2d::Node::init()) {
        return false;
    }
        
    _backing = cocos2d::Sprite::create();
    _icon = cocos2d::Sprite::create(icon);
        
    cocos2d::EventListenerTouchOneByOne *listener;
    listener = cocos2d::EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(CategoryButton::OnTouchBegan, this);
    listener->onTouchEnded = CC_CALLBACK_2(CategoryButton::OnTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(CategoryButton::OnTouchCancelled, this);
        
    auto director = cocos2d::Director::getInstance();
    auto dispatcher = director->getEventDispatcher();
    dispatcher->addEventListenerWithSceneGraphPriority(listener, this);
        
    setSelected(_selected);
    setScale(_normalScale);
    addChild(_backing, 0);
    addChild(_icon, 1);
        
    return true;
}
    
bool CategoryButton::OnTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    cocos2d::Vec2 worldPos = touch->getLocation();
    cocos2d::Vec2 localPos = convertToNodeSpace(worldPos);
    if (_backing->getBoundingBox().containsPoint(localPos))
    {
        setScale(_pressedScale);
        return true;
    }
    return false;
}
    
void CategoryButton::OnTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    cocos2d::Vec2 worldPos = touch->getLocation();
    cocos2d::Vec2 localPos = convertToNodeSpace(worldPos);
    if (_backing->getBoundingBox().containsPoint(localPos))
    {
        _selected = !_selected;
    }
    setScale(_normalScale);
    setSelected(_selected);
}
    
void CategoryButton::OnTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
    setScale(_normalScale);
    _backing->setTexture(_selectTex);
}
