//
//  PathSectorWidget.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 05/04/15.
//
//

#include "PathSectorWidget.h"

#include "GameInfo.h"

PathSectorWidget* PathSectorWidget::create(PathSector::WeakPtr path)
{
    PathSectorWidget *sector = new PathSectorWidget(path);
    if (sector && sector->init()) {
        sector->autorelease();
    } else {
        delete sector;
        sector = nullptr;
    }
    return sector;
}

PathSectorWidget::PathSectorWidget(PathSector::WeakPtr path)
: _path(path)
, _debugGrid(nullptr)
{
}

PathSectorWidget::~PathSectorWidget()
{
}

bool PathSectorWidget::init()
{
    if (!cocos2d::Node::init()) {
        return false;
    }
    
    PathSector::Ptr pathPtr = _path.lock();
    
    const float squareSize = GameInfo::Instance().GetFloat("SQUARE_SIZE");
    _sectorHeight = squareSize * pathPtr->GetSquaresByHeight();
    _sectorWidth = squareSize * 3.0f;
    
    cocos2d::Vec2 pathVerts[4] {
        cocos2d::Vec2(0.0f, 0.0f),
        cocos2d::Vec2(0.0f, _sectorHeight),
        cocos2d::Vec2(_sectorWidth, _sectorHeight),
        cocos2d::Vec2(_sectorWidth, 0.0f)
    };
    
    _ground = cocos2d::DrawNode::create();
    _ground->drawPolygon(pathVerts, 4, cocos2d::Color4F::GRAY, 0.0f, cocos2d::Color4F::GRAY);
    
    addChild(_ground, DrawOrder::GROUND);
    
    return true;
}

void PathSectorWidget::DrawDebugGrid()
{
    ClearDebugGrid();
    
    const cocos2d::Color4F color = cocos2d::Color4F::BLACK;
    const float squareSize = GameInfo::Instance().GetFloat("SQUARE_SIZE");
    const int squareByX = _path.lock()->GetSquaresByHeight();
    const int squareByY = 3.0f;
    
    _debugGrid = cocos2d::DrawNode::create();
    
    // draw vertical lines
    for (int k = 0; k < squareByX + 1; ++k) {
        const cocos2d::Vec2 origin(0.0f, k * squareSize);
        const cocos2d::Vec2 destination(_sectorWidth, k * squareSize);
        const bool borderLine = k == squareByX || k == 0;
        _debugGrid->drawLine(origin, destination, borderLine ? cocos2d::Color4F::RED : color);
    }
    
    // draw horizontal lines
    for (int k = 0; k < squareByY + 1; ++k) {
        const cocos2d::Vec2 origin(k * squareSize, 0.0f);
        const cocos2d::Vec2 destination(k * squareSize, _sectorHeight);
        _debugGrid->drawLine(origin, destination, color);
    }
    
    addChild(_debugGrid, DrawOrder::DEBUG_GREED);
}

void PathSectorWidget::ClearDebugGrid()
{
    if (_debugGrid) {
        removeChild(_debugGrid);
        _debugGrid->release();
        _debugGrid = nullptr;
    }
}

cocos2d::Size PathSectorWidget::GetSectorSize() const
{
    return cocos2d::Size(_sectorWidth, _sectorHeight);
}

PathSector::Ptr PathSectorWidget::GetPath() const
{
    return _path.lock();
}

