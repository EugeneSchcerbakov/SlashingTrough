//
//  FieldCamera.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 11/11/15.
//
//

#include "FieldCamera.h"

#include "GameInfo.h"
#include "Utils.h"

// we need this because of gettimeofday unix and windows compatibility
using namespace cocos2d;

FieldCamera* FieldCamera::create(cocos2d::CameraFlag flag)
{
    FieldCamera *camera = new FieldCamera();
    if (camera && camera->init(flag)) {
        camera->autorelease();
    } else {
        delete camera;
        camera = nullptr;
    }
    return camera;
}

FieldCamera::FieldCamera()
: cocos2d::Node()
, _interval(1.0 / 60.0)
{
}

FieldCamera::~FieldCamera()
{
}

bool FieldCamera::init(cocos2d::CameraFlag flag)
{
    if (!cocos2d::Node::init()) {
        return false;
    }
    
    auto gameinfo = GameInfo::getInstance();
    auto director = cocos2d::Director::getInstance();
    auto winSize = director->getWinSize();

    float viewAngle = gameinfo.getConstFloat("CAMERA_VIEW_ANGLE", 50);
    float viewHeight = gameinfo.getConstFloat("CAMERA_VIEW_HEIGHT", 1100.0f);
    float squareSize = gameinfo.getConstFloat("SQUARE_SIZE");
    float viewPosX = squareSize * 3.0f * 0.5f;
    float aspect = winSize.width / winSize.height;
    float fov = gameinfo.getConstFloat("CAMERA_FIELD_OF_VIEW", 50.0f);
    float zfar = gameinfo.getConstFloat("CAMERA_FAR_Z_CLIP_PLANE", 1000.0f);
    
    _yOffset = gameinfo.getConstFloat("CAMERA_Y_OFFSET", -850.0f);
    _lerpVelocity = gameinfo.getConstFloat("CAMERA_LERP_VELOCITY", 1.0f);
    
    _camera = cocos2d::Camera::createPerspective(fov, aspect, 0.1f, zfar);
    _camera->setCameraFlag(flag);
    _camera->setPosition3D(cocos2d::Vec3(viewPosX, 0.0f, viewHeight));
    _camera->setRotation3D(cocos2d::Vec3(viewAngle, 0.0f, 0.0f));
    _camera->setPositionY(_yOffset);
    
    addChild(_camera);
    scheduleUpdate();
    
	gettimeofday(&_lastUpdate, nullptr);
    
    return true;
}

void FieldCamera::update(float dt)
{
    timeval now;
    gettimeofday(&now, nullptr);
	
	auto director = cocos2d::Director::getInstance();
	auto scheduler = director->getScheduler();

    double update_delta = ((now.tv_sec - _lastUpdate.tv_sec) + (now.tv_usec - _lastUpdate.tv_usec) / 1000000.0) * 1000.0;
	update_delta *= scheduler->getTimeScale();

    bool flag = false;
    int cycles = (int)(update_delta / _interval);
    
    for (int k = 0; k < cycles; k++)
    {
        fixedUpdate();
        flag = true;
    }
    
    if (flag) {
        _lastUpdate = now;
    }
}

void FieldCamera::fixedUpdate()
{
    float y0 = _camera->getPositionY();
    float y1 = _targetPos.y + _yOffset;
    
    float t = math::clamp(_lerpVelocity, 0.0f, 1.0f);
    
    float delta = math::lerp(y0, y1, t);
    
    _camera->setPositionY(delta);
}

void FieldCamera::setTargetPosition(const cocos2d::Vec2 &pos)
{
    _targetPos = pos;
}

cocos2d::Camera* FieldCamera::getActualCamera()
{
    return _camera;
}
