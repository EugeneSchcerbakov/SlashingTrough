//
//  CharacterWidget.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 06/04/15.
//
//

#ifndef __SlashingTrough__CharacterWidget__
#define __SlashingTrough__CharacterWidget__

#include "cocos2d.h"
#include "Character.h"

class CharacterWidget : public cocos2d::Node
{
public:
    static CharacterWidget* create(Character::WeakPtr character);
    
protected:
    CharacterWidget(Character::WeakPtr character);
    virtual ~CharacterWidget();
    
    bool init();
    
    enum DrawOrder
    {
        BODY = 0,
        SWORD
    };
    
private:
    Character::WeakPtr _character;
    
    cocos2d::DrawNode *_sword;
    cocos2d::DrawNode *_body;
    cocos2d::DrawNode *_bodyBorder;
};

#endif /* defined(__SlashingTrough__CharacterWidget__) */
