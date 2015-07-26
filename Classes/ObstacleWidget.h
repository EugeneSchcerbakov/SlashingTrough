//
//  ObstacleWidget.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 12/07/15.
//
//

#ifndef __SlashingTrough__ObstacleWidget__
#define __SlashingTrough__ObstacleWidget__

#include "cocos2d.h"

#include "Obstacle.h"

class ObstacleWidget : public cocos2d::BillBoard
{
public:
    static ObstacleWidget* create(Obstacle *obstacle);
    
protected:
    ObstacleWidget(Obstacle *obstacle);
    virtual ~ObstacleWidget();
    
    bool init() override;
    
private:
    Obstacle *_obstacle;
};

#endif /* defined(__SlashingTrough__ObstacleWidget__) */
