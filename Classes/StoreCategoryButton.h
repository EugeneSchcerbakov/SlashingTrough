//
//  StoreCategoryButton.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 06/06/15.
//
//

#ifndef __SlashingTrough__StoreCategoryButton__
#define __SlashingTrough__StoreCategoryButton__

#include "cocos2d.h"
#include <string>
#include <functional>

class CategoryButton : public cocos2d::Node
{
public:
    static CategoryButton* create(const std::string &icon, const std::function<void(int)> &func);
    
    bool isSelected() const;
    void setSelected(bool flag);
    
protected:
    CategoryButton(const std::function<void(int)> &func);
    virtual ~CategoryButton();

    bool init(const std::string &icon);
    
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);

private:
    cocos2d::Sprite *_backing;
    cocos2d::Sprite *_icon;
    
    bool _selected;
    
    std::function<void(int)> _callback;
    
    const std::string _normalTex;
    const std::string _selectTex;
    const float _normalScale;
    const float _pressedScale;
};


#endif /* defined(__SlashingTrough__StoreCategoryButton__) */
