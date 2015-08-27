//
//  MapLevelMark.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 24/08/15.
//
//

#include "MapLevelMark.h"

MapLevelMark* MapLevelMark::create(FieldLevel::WeakPtr level)
{
    MapLevelMark *mark = new MapLevelMark();
    if (mark && mark->init(level)) {
        mark->autorelease();
    } else {
        delete mark;
        mark = nullptr;
    }
    return mark;
}

void MapLevelMark::setLevelText(const std::string &text)
{
    if (_text) {
        _text->setString(text);
    }
}

FieldLevel::WeakPtr MapLevelMark::getLevel() const
{
    return _level;
}

const std::string& MapLevelMark::getLevelId() const
{
    return _level->getId();
}

cocos2d::Rect MapLevelMark::getBoundingBox() const
{
    float w = _contentSize.width;
    float h = _contentSize.height;
    cocos2d::Rect rect(-w*0.5f, -h*0.5f, w, h);
    return RectApplyAffineTransform(rect, getNodeToParentAffineTransform());
}

MapLevelMark::MapLevelMark()
{
}

MapLevelMark::~MapLevelMark()
{
}

bool MapLevelMark::init(FieldLevel::WeakPtr level)
{
    if (level.expired() || !cocos2d::ui::Widget::init()) {
        return false;
    }
    
    _level = level.lock();
    
    _mark = cocos2d::Sprite::create("map/map_level_unlocked.png");
    _mark->setScale(1.5f);
    
    _text = cocos2d::ui::Text::create("", "font_prototype.ttf", 30);
    _text->setTextColor(cocos2d::Color4B::BLACK);
    _text->setTextHorizontalAlignment(cocos2d::TextHAlignment::CENTER);
    _text->setTextVerticalAlignment(cocos2d::TextVAlignment::CENTER);
    _text->setString("0");
    
    addChild(_mark, 0);
    addChild(_text, 1);
    setContentSize(_mark->getContentSize() * 1.5f);
    
    return true;
}
