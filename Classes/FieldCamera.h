//
//  FieldCamera.hpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 11/11/15.
//
//

#ifndef FieldCamera_h
#define FieldCamera_h

#include "cocos2d.h"

class FieldCamera : public cocos2d::Node
{
public:
    static FieldCamera* create(cocos2d::CameraFlag flag);
    
    void setTargetPosition(const cocos2d::Vec2 &pos);
    
    cocos2d::Camera* getActualCamera();
    
    void updateMagicParticlesCamera();
    
protected:
    FieldCamera();
    virtual ~FieldCamera();
    
    virtual bool init(cocos2d::CameraFlag flag);
    virtual void update(float dt) override;
    
private:
    void fixedUpdate();
    
    float _yOffset;
    float _lerpVelocity;
    const double _interval;
    timeval _lastUpdate;
    
    cocos2d::Vec2 _targetPos;
    
    cocos2d::Camera *_camera;
};

#endif /* FieldCamera_hpp */
