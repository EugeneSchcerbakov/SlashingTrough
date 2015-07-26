//
//  FieldSectorWidget.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 11/07/15.
//
//

#ifndef __SlashingTrough__FieldSectorWidget__
#define __SlashingTrough__FieldSectorWidget__

#include <cocos2d.h>

#include "FieldSector.h"

class FieldSectorWidget : public cocos2d::Sprite3D
{
public:
    static FieldSectorWidget* create(FieldSector::Ptr sector);
    
    FieldSector::Ptr getSector() const;
    
protected:
    FieldSectorWidget(FieldSector::Ptr sector);
    virtual ~FieldSectorWidget();
    
    bool init() override;
    
private:
    void createDebugGreed();
    
    FieldSector::Ptr _sector;
    
    cocos2d::Mesh *_mesh;
};

#endif /* defined(__SlashingTrough__FieldSectorWidget__) */
