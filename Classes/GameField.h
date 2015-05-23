//
//  GameField.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 05/04/15.
//
//

#ifndef __SlashingTrough__GameField__
#define __SlashingTrough__GameField__

#include "PathSectorWidget.h"
#include "HeroWidget.h"
#include "HeroControlKeyboard.h"
#include "HeroControlTouch.h"

#include "GameInterface.h"
#include "GameInfo.h"

class GameField : public cocos2d::Layer
{
public:
    static GameField* create(GameInterface *gameInterface);
    
    void SetSectorsQueueSize(int size);
    void SetScrollSpeed(float scrollSpeed);
    
    void Start();
    void Reset();
    
protected:
    GameField(GameInterface *gameInterface);
    virtual ~GameField();
    
    bool init();
    void update(float dt);
    
    void GenerateNewSector(bool makeEmpty = false);
    void UpdateDifficult();
    void RefreshInterface();
    
private:
    enum DrawOrder
    {
        PATH_CONTENT,
        HERO
    };
    
private:
    int _sectorsQueueSize;
    int _passedSectors;
    int _difficultIndex;
    
    GameplayObject::Ptr _hero;
    HeroWidget *_heroWidget;
    
    HeroControlKeyboard::Ptr _controlKeyboard;
    HeroControlTouch::Ptr _controlTouch;
    
    GameInfo::DifficultInfo _difficult;
    PathSectorWidget::SectorsSequence _sectorsSequence;
    
    GameInterface *_gameInterface;
};

#endif /* defined(__SlashingTrough__GameField__) */
