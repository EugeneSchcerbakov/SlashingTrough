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
#include "CharacterWidget.h"
#include "CharacterControlKeyboard.h"

#include "GameInfo.h"

class GameField : public cocos2d::Layer
{
public:
    static GameField* create();
    
    void SetSectorsQueueSize(int size);
    void SetScrollSpeed(float scrollSpeed);
    
    void Start();
    void Reset();
    
protected:
    GameField();
    virtual ~GameField();
    
    bool init();
    void update(float dt);
    
    PathSectorWidget* GenerateNewSector() const;
    void AddSector(PathSectorWidget *sectorWidget);
    void UpdateDifficult();
    
private:
    enum DrawOrder
    {
        PATH_CONTENT,
        CHARACTER
    };
    
private:
    int _sectorsQueueSize;
    int _passedSectors;
    int _difficultIndex;
    float _scrollSpeed;
    
    GameplayObject::Ptr _character;
    CharacterWidget *_characterWidget;
    CharacterControlKeyboard::Ptr _controlKeyboard;
    
    GameInfo::DiffucultInfo _difficult;
    PathSectorWidget::SectorsSequence _sectorsSequence;
};

#endif /* defined(__SlashingTrough__GameField__) */
