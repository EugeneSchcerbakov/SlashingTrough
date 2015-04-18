//
//  GameField.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 05/04/15.
//
//

#include "GameField.h"

GameField* GameField::create()
{
    GameField *field = new GameField();
    if (field && field->init()) {
        field->autorelease();
    } else {
        delete field;
        field = nullptr;
    }
    return field;
}

GameField::GameField()
: _scrollSpeed(400.0f)
, _sectorsQueueSize(3)
, _passedSectors(0)
, _difficultIndex(0)
{
}

GameField::~GameField()
{
}

void GameField::SetSectorsQueueSize(int size)
{
    _sectorsQueueSize = size;
}

void GameField::SetScrollSpeed(float scrollSpeed)
{
    _scrollSpeed = scrollSpeed;
}

void GameField::Start()
{
}

void GameField::Reset()
{
}

bool GameField::init()
{
    if (!cocos2d::Layer::init()) {
        return false;
    }
    
    GameInfo &gameinfo = GameInfo::Instance();
    
    if (!gameinfo.GetDiffucultSettings().empty()) {
        _difficultIndex = 0;
        _difficult = gameinfo.GetDiffucultSettings()[_difficultIndex];
    }
    
    _sectorsQueueSize = gameinfo.GetInt("SECTORS_SEQUENCE_MAX_SIZE");
    const float squareSize = gameinfo.GetFloat("SQUARE_SIZE");
    const int sectorSquaresCount = gameinfo.GetInt("SEСTOR_SQUARES_COUNT");
    
    for (int k = 0; k < _sectorsQueueSize; ++k)
    {
        PathSector::Ptr testPath = PathSector::Create();
        testPath->Generate(2, 3, sectorSquaresCount);
        PathSectorWidget *testPathWidget = PathSectorWidget::create(testPath);
        testPathWidget->setPosition(cocos2d::Vec2(0.0f, k * sectorSquaresCount * squareSize));
        testPathWidget->DrawDebugGrid();
        _sectorsSequence.push_back(testPathWidget);
        addChild(testPathWidget, DrawOrder::PATH_CONTENT);
    }
    
    const float characterStartX = squareSize * 3.0f * 0.5f;
    const float characterStartY = squareSize + squareSize * 0.5f;
    _character = Character::Create();
    _character->SetLogicalPos(characterStartX, characterStartY);
    _characterWidget = CharacterWidget::create(_character);
    _characterWidget->setPositionX(_character->GetLogicalX());
    _characterWidget->setPositionY(_character->GetLogicalY());
    
    _controlKeyboard = CharacterControlKeyboard::Create(_character,
                                                        _characterWidget->getEventDispatcher(),
                                                        _characterWidget);
    addChild(_characterWidget, DrawOrder::CHARACTER);
    
    scheduleUpdate();
    
    return true;
}

void GameField::update(float dt)
{
    SetScrollSpeed(_character->GetRunningSpeed());
    
    cocos2d::Vec2 origin = cocos2d::Director::getInstance()->getVisibleOrigin();
    
    // scroll level down
    for (SectorsSequenceIter it = _sectorsSequence.begin(); it != _sectorsSequence.end(); ++it)
    {
        PathSectorWidget *sector = (*it);
        float ypos = sector->getPositionY();
        ypos -= _scrollSpeed * dt;
        sector->setPositionY(ypos);
    }
    
    // create new and delete passed sectors
    for (SectorsSequenceIter it = _sectorsSequence.begin(); it != _sectorsSequence.end();)
    {
        PathSectorWidget *sector = (*it);
        cocos2d::Vec2 localPos = sector->getPosition();
        cocos2d::Vec2 upperPos = localPos + cocos2d::Vec2(0.0f, sector->GetSectorSize().height);
        cocos2d::Vec2 worldPos = convertToWorldSpace(upperPos);
        if (worldPos.y < origin.y)
        {
            removeChild(sector, true);
            it = _sectorsSequence.erase(it);
            PathSectorWidget *newSector = GenerateNewSector();
            
            AddSector(newSector);
            _passedSectors++;
            UpdateDifficult();
        } else
            ++it;
    }
}

PathSectorWidget* GameField::GenerateNewSector() const
{
    const int sectorSquaresCount = GameInfo::Instance().GetInt("SEСTOR_SQUARES_COUNT");
    
    PathSector::Ptr sector = PathSector::Create();
    sector->Generate(_difficult.obstacles, _difficult.enemies, sectorSquaresCount);
    PathSectorWidget *sectorWidget = PathSectorWidget::create(sector);
    sectorWidget->DrawDebugGrid();
    sectorWidget->setPosition(cocos2d::Vec2(0.0f, 0.0f));
    return sectorWidget;
}

void GameField::AddSector(PathSectorWidget *sectorWidget)
{
    if (!sectorWidget) {
        return;
    }

    float ypos = 0.0f;
    if (!_sectorsSequence.empty())
    {
        SectorsSequenceIter lastSector = --_sectorsSequence.end();
        float lastSectorY = (*lastSector)->getPositionY();
        ypos = lastSectorY + sectorWidget->GetSectorSize().height;
    }
    
    sectorWidget->setPositionY(ypos);
    _sectorsSequence.push_back(sectorWidget);
    addChild(sectorWidget, DrawOrder::PATH_CONTENT);
}

void GameField::UpdateDifficult()
{
    GameInfo &gameinfo = GameInfo::Instance();
    const GameInfo::DiffucultSettings &settings = gameinfo.GetDiffucultSettings();
    if (_passedSectors >= _difficult.sectors) {
        _passedSectors = 0;
        ++_difficultIndex;
        if (_difficultIndex < settings.size()) {
            _difficult = settings[_difficultIndex];
        }
    }
}
