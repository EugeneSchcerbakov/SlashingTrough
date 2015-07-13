//
//  FieldSectorWidget.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 11/07/15.
//
//

#include "FieldSectorWidget.h"
#include "GameInfo.h"

FieldSectorWidget* FieldSectorWidget::create(FieldSector::Ptr sector)
{
    FieldSectorWidget *widget = new FieldSectorWidget(sector);
    if (widget && widget->init()) {
        widget->autorelease();
    } else {
        delete widget;
        widget = nullptr;
    }
    return widget;
}

FieldSectorWidget::FieldSectorWidget(FieldSector::Ptr sector)
: _sector(sector)
, _debugGrid(nullptr)
{
}

FieldSectorWidget::~FieldSectorWidget()
{
}

bool FieldSectorWidget::init()
{
    if (!cocos2d::Node::init()) {
        return false;
    }
    
    float w = _sector->getWidth();
    float h = _sector->getHeight();
    
    cocos2d::Vec2 verices[4] {
        cocos2d::Vec2(0.0f, 0.0f),
        cocos2d::Vec2(0.0f, h),
        cocos2d::Vec2(w, h),
        cocos2d::Vec2(w, 0.0f)
    };
    
    _ground = cocos2d::DrawNode::create();
    _ground->drawPolygon(verices, 4, cocos2d::Color4F::GRAY, 0.0f, cocos2d::Color4F::GRAY);
    
    bool drawDebugGreed = true;
    if (drawDebugGreed) {
        createDebugGreed();
    }
    
    addChild(_ground, 0);
    
    return true;
}

FieldSector::Ptr FieldSectorWidget::getSector() const
{
    return _sector;
}

void FieldSectorWidget::createDebugGreed()
{
    if (_debugGrid) {
        removeChild(_debugGrid);
        _debugGrid->release();
        _debugGrid = nullptr;
    }
    
    cocos2d::Color4F color = cocos2d::Color4F::BLACK;
    float squareSize = GameInfo::getInstance().getFloat("SQUARE_SIZE");
    int squaresByX = _sector->getSquaresNumByX();
    int squaresByY = _sector->getSquaresNumByY();
    
    _debugGrid = cocos2d::DrawNode::create();
    
    // draw vertical lines
    for (int k = 0; k < squaresByX + 1; k++) {
        const cocos2d::Vec2 origin(k * squareSize, 0.0f);
        const cocos2d::Vec2 destination(k * squareSize, _sector->getHeight());
        _debugGrid->drawLine(origin, destination, color);
    }
    
    // draw horizontal lines
    for (int k = 0; k < squaresByY; k++) {
        cocos2d::Vec2 origin(0.0, k * squareSize);
        cocos2d::Vec2 destination(_sector->getWidth(), k * squareSize);
        bool border = k == squaresByY || k == 0;
        _debugGrid->drawLine(origin, destination, border ? cocos2d::Color4F::RED : color);
    }
    
    addChild(_debugGrid, 1);
}
