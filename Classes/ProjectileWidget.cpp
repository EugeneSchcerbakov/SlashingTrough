//
//  ProjectileWidget.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 14/07/15.
//
//

#include "ProjectileWidget.h"

ProjectileWidget* ProjectileWidget::create(Projectile *projectile)
{
    ProjectileWidget *widget = new ProjectileWidget(projectile);
    if (widget && widget->init()) {
        widget->autorelease();
    } else {
        delete widget;
        widget = nullptr;
    }
    return widget;
}

ProjectileWidget::ProjectileWidget(Projectile *projectile)
: _projectile(projectile)
{
}

ProjectileWidget::~ProjectileWidget()
{
}

bool ProjectileWidget::init()
{
    if (!cocos2d::BillBoard::init()) {
        return false;
    }
    
    _projectile->setupAccepter(accepter, static_cast<void *>(this));
    
    _rotationDir = rand() % 100 > 50 ? 1.0f : -1.0f;
    
    _sprite = cocos2d::Sprite::create(_projectile->getSpriteFilename());
    
    addChild(_sprite);
    scheduleUpdate();
    setMode(cocos2d::BillBoard::Mode::VIEW_PLANE_ORIENTED);
	setPositionZ(_sprite->getTexture()->getContentSize().height);
    
    if (_projectile) {
        float x = _projectile->getPositionX();
        float y = _projectile->getPositionY();
        setPositionX(x);
        setPositionY(y);
    }
    
    return true;
}

void ProjectileWidget::update(float dt)
{
    if (_projectile) {
        setPositionX(_projectile->getPositionX());
        setPositionY(_projectile->getPositionY());
    }
    
    float rotationSpeed = 700.0f;
    float angle = getRotation();
    angle += rotationSpeed * dt * _rotationDir;
    _sprite->setRotation(angle);
}


void ProjectileWidget::acceptEvent(const Event &event)
{
}

void ProjectileWidget::accepter(const Event &event, void *param)
{
    static_cast<ProjectileWidget *>(param)->acceptEvent(event);
}
