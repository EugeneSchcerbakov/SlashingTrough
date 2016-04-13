//
//  ProjectileWidget.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 14/07/15.
//
//

#ifndef __SlashingTrough__ProjectileWidget__
#define __SlashingTrough__ProjectileWidget__

#include <cocos2d.h>

#include "Projectile.h"

class ProjectileWidget : public cocos2d::BillBoard
{
public:
    static ProjectileWidget* create(Projectile *projectile);
    
protected:
    ProjectileWidget(Projectile *projectile);
    virtual ~ProjectileWidget();
    
    bool init() override;
    void update(float dt) override;
    
private:
    Projectile *_projectile;
    cocos2d::Sprite *_sprite;
    
    float _rotationDir;
};

#endif /* defined(__SlashingTrough__ProjectileWidget__) */
