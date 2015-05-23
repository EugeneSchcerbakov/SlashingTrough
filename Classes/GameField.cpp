//
//  GameField.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 05/04/15.
//
//

#include "GameField.h"
#include "Utils.h"

GameField* GameField::create(GameInterface *gameInterface)
{
    GameField *field = new GameField(gameInterface);
    if (field && field->init()) {
        field->autorelease();
    } else {
        delete field;
        field = nullptr;
    }
    return field;
}

GameField::GameField(GameInterface *gameInterface)
: _gameInterface(gameInterface)
, _sectorsQueueSize(3)
, _passedSectors(0)
, _difficultIndex(0)
{
}

GameField::~GameField()
{
    getEventDispatcher()->removeCustomEventListeners("RefreshInterface");
}

void GameField::SetSectorsQueueSize(int size)
{
    _sectorsQueueSize = size;
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
    const float heroStartX = squareSize * 3.0f * 0.5f;
    const float heroStartY = squareSize + squareSize * 0.5f;
    
    _hero = Hero::Create();
    _hero->SetLogicalPos(heroStartX, heroStartY);
    _heroWidget = HeroWidget::create(_hero);
    _heroWidget->setPositionX(_hero->GetLogicalX());
    _heroWidget->setPositionY(_hero->GetLogicalY());

    for (int k = 0; k < _sectorsQueueSize; ++k) {
        bool makeEmpty = false;
        if (k == 0) {
            // first sector must be empty
            makeEmpty = true;
        }
        GenerateNewSector(makeEmpty);
    }
    
    _controlKeyboard = HeroControlKeyboard::Create(_hero, _heroWidget->getEventDispatcher(), _heroWidget);
    _controlTouch = HeroControlTouch::Create(_hero, _heroWidget->getEventDispatcher(), _heroWidget);
    
    getEventDispatcher()->addCustomEventListener("RefreshInterface", [&](cocos2d::EventCustom*){RefreshInterface();});
    addChild(_heroWidget, DrawOrder::HERO);
    scheduleUpdate();
    
    RefreshInterface();
    
    return true;
}

void GameField::update(float dt)
{
    Hero *hero = Hero::Cast(_hero);
    
    if (!_hero->IsAlive())
    {
        int goldPoints = hero->GetGoldPoints();
        int killPoints = hero->GetKillPoints();
        
        Utils::LuaSetGlobalInteger("PlayerResultGoldPoints", goldPoints);
        Utils::LuaSetGlobalInteger("PlayerResultKillPoints", killPoints);
        auto scene = Utils::MakeSceneFromLua("CreateResultScene");
        
        cocos2d::Director *director;
        director = cocos2d::Director::getInstance();
        director->replaceScene(scene);
        return;
    }
    
    hero->SetRunningSpeed(_difficult.speed);
    hero->IdleUpdate(dt);
    _heroWidget->RefreshSectorsSequence(_sectorsSequence);
    
    // scroll level down
    for (PathSectorWidget::SectorsSequenceIter it = _sectorsSequence.begin(); it != _sectorsSequence.end(); ++it)
    {
        PathSectorWidget *sector = (*it);
        float ypos = sector->GetSnapPos().y - hero->GetYPosOnRoad();
        sector->setPositionY(ypos);
    }
    
    cocos2d::Vec2 origin = cocos2d::Director::getInstance()->getVisibleOrigin();
    
    // create new and delete passed sectors
    for (PathSectorWidget::SectorsSequenceIter it = _sectorsSequence.begin(); it != _sectorsSequence.end();)
    {
        PathSectorWidget *sector = (*it);
        cocos2d::Vec2 lowerPos = sector->getPosition();
        cocos2d::Vec2 upperPos = lowerPos + cocos2d::Vec2(0.0f, sector->GetSectorSize().height);
        cocos2d::Vec2 worldPos = convertToWorldSpace(upperPos);
        if (worldPos.y < origin.y)
        {
            removeChild(sector, true);
            it = _sectorsSequence.erase(it);
            GenerateNewSector();
            _passedSectors++;
        } else
            ++it;
    }
}

void GameField::GenerateNewSector(bool makeEmpty)
{
    const int sectorSquaresCount = GameInfo::Instance().GetInt("SEСTOR_SQUARES_COUNT");
    PathSector::Ptr sector = PathSector::Create();
    sector->Generate(sectorSquaresCount);
    if (!makeEmpty) {
        sector->SpawnObjects(_difficult.obstaclesPerSector, _difficult.enemiesPerSector);
    }
    
    PathSectorWidget *sectorWidget = PathSectorWidget::create(sector, _heroWidget);
    sectorWidget->DrawDebugGrid();
    sectorWidget->setPosition(cocos2d::Vec2(0.0f, 0.0f));
    
    float ypos = 0.0f;
    if (!_sectorsSequence.empty())
    {
        PathSectorWidget::SectorsSequenceIter lastSector = --_sectorsSequence.end();
        float lastSectorY = (*lastSector)->GetSnapPos().y;
        ypos = lastSectorY + sectorWidget->GetSectorSize().height;
    }
    
    sectorWidget->setPositionY(ypos);
    sectorWidget->SetSnapPos(cocos2d::Vec2(0.0f, ypos));
    _sectorsSequence.push_back(sectorWidget);
    addChild(sectorWidget, DrawOrder::PATH_CONTENT);
    
    UpdateDifficult();
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

void GameField::RefreshInterface()
{
    Hero *hero = Hero::Cast(_hero);
    _gameInterface->SetGoldPointsLabel(hero->GetGoldPoints());
    _gameInterface->SetKillPointsLabel(hero->GetKillPoints());
    _gameInterface->SetDamagePointsLabel(hero->GetDamagePoints());
    
    int totalHealth = (int)GameInfo::Instance().GetFloat("HERO_HEALTH_POINTS");
    int currentHealth = (int)hero->GetHealth();
    int percentHealth = (currentHealth * 100) / totalHealth;
    _gameInterface->SetHealthPointsLabel(percentHealth);
    
    float maxStaminaPoints = GameInfo::Instance().GetFloat("HERO_STAMINA_POINTS");
    float curStaminaPoints = hero->GetStaminaPoints();
    _gameInterface->SetStaminaPoints(curStaminaPoints / maxStaminaPoints);
}

