//
//  ObstacleWidget.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 12/07/15.
//
//

#include "ObstacleWidget.h"

ObstacleWidget* ObstacleWidget::create(Obstacle *obstacle)
{
    ObstacleWidget *widget = new ObstacleWidget(obstacle);
    if (widget && widget->init()) {
        widget->autorelease();
    } else {
        delete widget;
        widget = nullptr;
    }
    return widget;
}

ObstacleWidget::ObstacleWidget(Obstacle *obstacle)
: _obstacle(obstacle)
{
    CC_ASSERT(_obstacle);
}

ObstacleWidget::~ObstacleWidget()
{
}

bool ObstacleWidget::init()
{
    if (!cocos2d::BillBoard::init()) {
        return false;
    }
    
    setTexture(_obstacle->getSpriteFilename());
    setMode(cocos2d::BillBoard::Mode::VIEW_PLANE_ORIENTED);
    setPositionX(_obstacle->getPositionX());
    setPositionY(_obstacle->getPositionY());
    setPositionZ(getTexture()->getContentSize().height * 0.5f);
    
    return true;
}