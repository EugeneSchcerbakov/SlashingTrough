//
//  FieldLayer.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 11/07/15.
//
//

#ifndef __SlashingTrough__FieldLayer__
#define __SlashingTrough__FieldLayer__

#include <cocos2d.h>

#include "Field.h"

#include "FieldSectorWidget.h"
#include "HeroWidget.h"
#include "ProjectileWidget.h"
#include "ObstacleWidget.h"
#include "EnemyWidget.h"
#include "FieldCamera.h"
#include "EffectsLayer.h"

#include "ControlKeyboard.h"
#include "ControlTouch.h"

#include "GameInterface.h"

class FieldLayer : public cocos2d::Layer
{
public:
    static FieldLayer* create(const std::string &levelId, GameInterface *gameInterface);
    
protected:
    FieldLayer(GameInterface *gameInterface);
    virtual ~FieldLayer();
    
    virtual void visit(cocos2d::Renderer *renderer, const cocos2d::Mat4 &parentTransform, uint32_t parentFlags) override;
    
    bool init(const std::string &levelId);
    void update(float dt) override;
    void refreshInterface();
    
    void acceptEvent(const Event &event);
    
private:
    static void accepter(const Event &event, void *param);

    void makeLevelComplete(FieldLevel::WeakPtr levelPtr);
    
    Field _field;
    
    std::vector<EnemyWidget *> _enemiesWidgets;
    
    cocos2d::experimental::FrameBuffer *_fboColor;
    cocos2d::experimental::FrameBuffer *_fboDistor;
    cocos2d::experimental::RenderTarget *_rtSceneDistor;
    cocos2d::experimental::RenderTarget *_rtSceneColor;
    cocos2d::experimental::RenderTargetDepthStencil *_rtSceneDepth;
    cocos2d::Sprite *_rtColorBuffer;
    
    cocos2d::GLProgram *_distortionShader;
    cocos2d::GLProgramState *_distortionState;
    
    EffectsLayer *_fieldEffects;
    
    FieldCamera *_fieldCamera;
    FieldCamera *_distorCamera;
    
    GameInterface *_gameInterface;
    HeroWidget *_heroWidget;
    
    ControlTouch::Ptr _controlTouch;
    ControlKeyboard::Ptr _controlKeyboard;
    
    float _cameraYOffset;
};

#endif /* defined(__SlashingTrough__FieldLayer__) */
