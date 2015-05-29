//
//  PathSectorWidget.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 05/04/15.
//
//

#include "PathSectorWidget.h"

#include "Utils.h"
#include "GameInfo.h"
#include "HeroWidget.h"
#include "GameplayObjectsWidgets.h"

PathSectorWidget* PathSectorWidget::create(PathSector::Ptr path, HeroWidget *hero)
{
    PathSectorWidget *sector = new PathSectorWidget(path, hero);
    if (sector && sector->init()) {
        sector->autorelease();
    } else {
        delete sector;
        sector = nullptr;
    }
    return sector;
}

PathSectorWidget::PathSectorWidget(PathSector::Ptr path, HeroWidget *hero)
: _path(path)
, _heroWidget(hero)
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
    
    const float squareSize = GameInfo::Instance().GetFloat("SQUARE_SIZE");
    _sectorHeight = squareSize * _path->GetSquaresByHeight();
    _sectorWidth = squareSize * 3.0f;
    
    for (GameplayObject::Ptr object : _path->GetGameplayObjects())
    {
        if (object->IsType(GameplayObject::Type::OBSTACLE)) {
            ObstacleWidget *widget = ObstacleWidget::create(object);
            widget->setPositionX(object->GetLogicalX());
            widget->setPositionY(object->GetLogicalY());
            widget->setScale(1.0f);
            addChild(widget, DrawOrder::OBSTACLES);
            _widgets[object->GetUID()] = widget;
        } else if (object->IsType(GameplayObject::Type::ENEMY)) {
            EnemyWidget *widget = EnemyWidget::create(object);
            widget->setPositionX(object->GetLogicalX());
            widget->setPositionY(object->GetLogicalY());
            widget->setScale(1.0f);
            addChild(widget, DrawOrder::ENEMIES);
            _widgets[object->GetUID()] = widget;
        }
    }
    
    cocos2d::Vec2 pathVerts[4] {
        cocos2d::Vec2(0.0f, 0.0f),
        cocos2d::Vec2(0.0f, _sectorHeight),
        cocos2d::Vec2(_sectorWidth, _sectorHeight),
        cocos2d::Vec2(_sectorWidth, 0.0f)
    };
    
    _ground = cocos2d::DrawNode::create();
    _ground->drawPolygon(pathVerts, 4, cocos2d::Color4F::GRAY, 0.0f, cocos2d::Color4F::GRAY);
    
    scheduleUpdate();
    addChild(_ground, DrawOrder::GROUND);
    
    return true;
}

void PathSectorWidget::update(float dt)
{
    PathSector::GameplayObjects &objects = _path->GetGameplayObjects();
    
    // delete dead objects
    for (PathSector::GameplayObjectsIter it = objects.begin(); it != objects.end();) {
        if ((*it)->IsAlive()) {
            ++it;
        } else {
            // remove widget
            std::map<GameplayObject::UID, GameplayObjectWidget *>::iterator witer;
            witer = _widgets.find((*it)->GetUID());
            if (witer != _widgets.end()) {
                removeChild((*witer).second);
                witer = _widgets.erase(witer);
            }
            // remove object
            it = objects.erase(it);
        }
    }
    
    // update objects attack
    cocos2d::Vec2 playerLocal = convertToNodeSpace(_heroWidget->getPosition());
    float squareSize = GameInfo::Instance().GetFloat("SQUARE_SIZE");
    int playerSquareIndex = floorf(playerLocal.x / squareSize);
    
    for (GameplayObject::Ptr object : objects)
    {
        cocos2d::Vec2 local(object->GetLogicalX(), object->GetLogicalY());
        PathSector::Square square = _path->GetSquareByLocalXY(local.x, local.y);
        if (square.x == playerSquareIndex && object->GetLogicalY() > playerLocal.y)
        {
            float D = local.distance(playerLocal);
            GameplayObject::Ptr ptr = _heroWidget->GetHero().lock();
            if (object->Attack(ptr, D))
            {
                object->Kill();
                _heroWidget->RunEffectReceiveDamage();
                Hero::Cast(ptr)->AddStaminaPoints(-object->GetStaminaDrainPoints());
                getEventDispatcher()->dispatchCustomEvent("RefreshInterface");
            }
        }
    }
}

void PathSectorWidget::DrawDebugGrid()
{
    ClearDebugGrid();
    
    const cocos2d::Color4F color = cocos2d::Color4F::BLACK;
    const float squareSize = GameInfo::Instance().GetFloat("SQUARE_SIZE");
    const int squareByX = _path->GetSquaresByHeight();
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
    return _path;
}

GameplayObjectWidget* PathSectorWidget::GetObjectWidget(GameplayObject::UID uid)
{
    std::map<GameplayObject::UID, GameplayObjectWidget *>::iterator iter;
    iter = _widgets.find(uid);
    if (iter != _widgets.end()) {
        return iter->second;
    }
    return nullptr;
}
