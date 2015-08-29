//
//  MapLevelMark.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 24/08/15.
//
//

#ifndef __SlashingTrough__MapLevelMark__
#define __SlashingTrough__MapLevelMark__

#include "cocos2d.h"
#include "ui/CocosGUI.h"

#include "FieldLevel.h"

class MapLevelMark : public cocos2d::ui::Widget
{
public:
    static MapLevelMark* create(FieldLevel::WeakPtr level);
    
    void setLevelText(const std::string &text);
    
    FieldLevel::WeakPtr getLevel() const;
    const std::string& getLevelId() const;
    virtual cocos2d::Rect getBoundingBox() const override;
    
protected:
    MapLevelMark();
    virtual ~MapLevelMark();
    
    bool init(FieldLevel::WeakPtr level);
    void update(float dt);
    
protected:
    std::string makeIconForStatus(FieldLevel::Status status) const;
    
    FieldLevel::Ptr _level;
    FieldLevel::Status _lastStatus;
    
    cocos2d::ui::Text *_text;
    cocos2d::Sprite *_mark;
};

#endif /* defined(__SlashingTrough__MapLevelMark__) */
