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

#include "ControlKeyboard.h"
#include "ControlTouch.h"

#include "GameInterface.h"

class FieldLayer : public cocos2d::Layer
{
public:
    static FieldLayer* create(GameInterface *gameInterface);
    static const float heroYOffsetOnScreen; // offset from bottom of the screen
    
protected:
    FieldLayer(GameInterface *gameInterface);
    virtual ~FieldLayer();
    
    bool init() override;
    void update(float dt) override;
    void refreshInterface();
    
    void acceptEvent(const Event &event);
    
private:
    enum DrawOrder
    {
        GROUND = 0,
        OBSTACLES,
        ENEMIES,
        PROJECTILES,
        HERO
    };
    
private:
    static void accepter(const Event &event, void *param);

    Field _field;
    
    GameInterface *_gameInterface;
    EffectsLayer *_effectsOnField;
    cocos2d::Node *_fieldScroller;
    HeroWidget *_heroWidget;
    
    ControlTouch::Ptr _controlTouch;
    ControlKeyboard::Ptr _controlKeyboard;
};

#endif /* defined(__SlashingTrough__FieldLayer__) */
