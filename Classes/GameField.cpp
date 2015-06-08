//
//  GameField.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 05/04/15.
//
//

#include "GameField.h"
#include "SessionInfo.h"
#include "Store.h"
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

void GameField::AddEffectOnField(Effect *effect)
{
    _effectsGameField->AddEffect(effect);
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

cocos2d::Vec2 GameField::ConvertToRoadSpace(const cocos2d::Vec2 point)
{
    return _roadBasis->convertToNodeSpace(point);
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
    
    _effectsGameField = EffectsLayer::create();
    
    _roadBasis = cocos2d::Node::create();
    _roadBasis->addChild(_effectsGameField, 3);
    
    _hero = Hero::Create();
    _hero->SetLogicalPos(heroStartX, heroStartY);
    _heroWidget = HeroWidget::create(_hero, this);
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
    
    auto handler = [&](cocos2d::EventCustom*){RefreshInterface();};
    auto dispatcher = getEventDispatcher();
    dispatcher->addCustomEventListener("RefreshInterface", handler);
    
    addChild(_heroWidget, DrawOrder::HERO);
    addChild(_roadBasis, DrawOrder::PATH_CONTENT);
    scheduleUpdate();
    
    RefreshInterface();
    
    return true;
}

void GameField::update(float dt)
{
    Hero *hero = Hero::Cast(_hero);
    
    if (!_hero->IsAlive())
    {
        OnHeroKilled();
        return;
    }
    
    hero->SetRunningSpeed(_difficult.speed);
    hero->IdleUpdate(dt);
    _heroWidget->RefreshSectorsSequence(_sectorsSequence);
    _roadBasis->setPositionY(-hero->GetYPosOnRoad()); // scroll level down
    
    cocos2d::Vec2 origin = cocos2d::Director::getInstance()->getVisibleOrigin();
    
    // create new and delete passed sectors
    for (PathSectorWidget::SectorsSequenceIter it = _sectorsSequence.begin(); it != _sectorsSequence.end();)
    {
        PathSectorWidget *sector = (*it);
        cocos2d::Vec2 lowerPos = _roadBasis->convertToWorldSpace(sector->getPosition());
        cocos2d::Vec2 upperPos = lowerPos + cocos2d::Vec2(0.0f, sector->GetSectorSize().height);
        cocos2d::Vec2 worldPos = upperPos;
        if (worldPos.y < origin.y)
        {
            _roadBasis->removeChild(sector, true);
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
        float lastSectorY = (*lastSector)->getPositionY();
        ypos = lastSectorY + sectorWidget->GetSectorSize().height;
    }
    
    sectorWidget->setPositionY(ypos);
    _sectorsSequence.push_back(sectorWidget);
    _roadBasis->addChild(sectorWidget);
    
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
    _gameInterface->SetGoldPointsLabel(hero->GetScore().coins);
    _gameInterface->SetKillPointsLabel(hero->GetScore().kills);
    _gameInterface->SetDamagePointsLabel(hero->GetDamagePoints());
    
    int totalHealth = (int)GameInfo::Instance().GetFloat("HERO_HEALTH_POINTS");
    int currentHealth = (int)hero->GetHealth();
    int percentHealth = (currentHealth * 100) / totalHealth;
    _gameInterface->SetHealthPointsLabel(percentHealth);
    
    float maxStaminaPoints = GameInfo::Instance().GetFloat("HERO_STAMINA_POINTS");
    float curStaminaPoints = hero->GetStaminaPoints();
    _gameInterface->SetStaminaPoints(curStaminaPoints / maxStaminaPoints);
}

void GameField::OnHeroKilled()
{
    Hero *hero = Hero::Cast(_hero);
    
    SessionInfo &session = SessionInfo::Instance();
    SessionInfo::Score score = hero->GetScore();
    session.AddCoins(score.coins);
    if (session.IsBestScore(score)) {
        session.SetBestScore(score);
    }
    
    Equip::Ptr ptr = Store::Instance().GetItemById(session.GetEquippedWeaponId());
    EquipWeapon *wpn = EquipWeapon::cast(ptr);
    
    Utils::LuaSetGlobalInteger("PlayerTotalGoldPoints", session.GetCoins());
    Utils::LuaSetGlobalInteger("PlayerTotalDamagePoints", (int)wpn->damage);
    Utils::LuaSetGlobalInteger("PlayerBestResultGoldPoints", session.GetBestScore().coins);
    Utils::LuaSetGlobalInteger("PlayerBestResultKillPoints", session.GetBestScore().kills);
    Utils::LuaSetGlobalInteger("PlayerResultGoldPoints", score.coins);
    Utils::LuaSetGlobalInteger("PlayerResultKillPoints", score.kills);
    auto scene = Utils::MakeSceneFromLua("CreateResultScene");
    
    cocos2d::Director *director;
    director = cocos2d::Director::getInstance();
    director->replaceScene(scene);
    
    session.Save();
}
