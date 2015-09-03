//
//  MapLevelMark.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 24/08/15.
//
//

#include "MapLevelMark.h"

#include "Log.h"

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

const std::string& MapLevelMark::getLevelText() const
{
    return _text->getString();
}

cocos2d::Rect MapLevelMark::getBoundingBox() const
{
    float w = _contentSize.width;
    float h = _contentSize.height;
    cocos2d::Rect rect(-w*0.5f, -h*0.5f, w, h);
    return RectApplyAffineTransform(rect, getNodeToParentAffineTransform());
}

void MapLevelMark::setPosition(const cocos2d::Vec2& position)
{
    float x = position.x - getContentSize().width * 0.5f;
    float y = position.y - getContentSize().height * 0.5f;
    _level->setPosOnMap(x, y);
    
    cocos2d::ui::Widget::setPosition(position);
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
    _lastStatus = _level->getStatus();
    
    _mark = cocos2d::Sprite::create("map/map_level_unlocked.png");
    _mark->setScale(1.5f);
    
    _text = cocos2d::ui::Text::create("", "font_prototype.ttf", 30);
    _text->setTextColor(cocos2d::Color4B::BLACK);
    _text->setTextHorizontalAlignment(cocos2d::TextHAlignment::CENTER);
    _text->setTextVerticalAlignment(cocos2d::TextVAlignment::CENTER);
    _text->setString("0");
    
    _mark->setTexture(makeIconForStatus(_lastStatus));
    _text->setVisible(!_level->isStatus(FieldLevel::Status::LOCKED));
    
    addChild(_mark, 0);
    addChild(_text, 1);
    setContentSize(_mark->getContentSize() * 1.5f);
    scheduleUpdate();
    
    return true;
}

void MapLevelMark::update(float dt)
{
    if (_lastStatus != _level->getStatus()) {
        _lastStatus = _level->getStatus();
        _mark->setTexture(makeIconForStatus(_lastStatus));
        _text->setVisible(!_level->isStatus(FieldLevel::Status::LOCKED));
    }
}

std::string MapLevelMark::makeIconForStatus(FieldLevel::Status status) const
{
    if (status == FieldLevel::Status::LOCKED) {
        return "map/map_level_locked.png";
    } else if (status == FieldLevel::Status::UNLOCKED) {
        return "map/map_level_unlocked.png";
    } else if (status == FieldLevel::Status::COMPLETED) {
        return "map/map_level_unlocked.png";
    } else {
        return "map/map_level_unlocked.png";
        WRITE_WARN("Unknown level status.");
    }
}
