//
//  FieldLayer.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 11/07/15.
//
//

#include "FieldLayer.h"
#include "GameInfo.h"
#include "SessionInfo.h"
#include "Utils.h"

const float FieldLayer::heroYOffsetOnScreen = 250.0f;

FieldLayer* FieldLayer::create(GameInterface *gameInterface)
{
    FieldLayer *layer = new FieldLayer(gameInterface);
    if (layer && layer->init()) {
        layer->autorelease();
    } else {
        delete layer;
        layer = nullptr;
    }
    return layer;
}

FieldLayer::FieldLayer(GameInterface *gameInterface)
: _gameInterface(gameInterface)
{
}

FieldLayer::~FieldLayer()
{
    _field.finalize();
    _controlTouch->free();
    _controlKeyboard->free();
}

bool FieldLayer::init()
{
    if (!cocos2d::Layer::init()) {
        return false;
    }
    
    _fieldScroller = cocos2d::Node::create();
    
    _effectsOnField = EffectsLayer::create();
    _fieldScroller->addChild(_effectsOnField, 3);
    
    _field.setupAccepter(accepter, static_cast<void *>(this));
    _field.initialize();
    
    Hero *hero = _field.getHero();
    _heroWidget = HeroWidget::create(_field.getHero());
    _heroWidget->setPositionX(hero->getPositionX());
    _heroWidget->setPositionY(heroYOffsetOnScreen);
    
    auto dispatcher = cocos2d::Director::getInstance()->getEventDispatcher();
    
    _controlTouch = ControlTouch::create(_field.getHero(), dispatcher, this);
    _controlKeyboard = ControlKeyboard::create(_field.getHero(), dispatcher, this);
    
    addChild(_fieldScroller, DrawOrder::GROUND);
    addChild(_heroWidget, DrawOrder::HERO);
    scheduleUpdate();
    
    return true;
}

void FieldLayer::update(float dt)
{
    _field.idleUpdate(dt);
    
    Hero *hero = _field.getHero();
    if (hero) {
        float posx = hero->getPositionX();
        _heroWidget->setPositionX(posx);
        _fieldScroller->setPositionY(-hero->getPositionY() + heroYOffsetOnScreen);
    }
    
    refreshInterface();
}

void FieldLayer::refreshInterface()
{
    Hero *hero = _field.getHero();
    
    if (hero)
    {
        SessionInfo::Score score = hero->getScore();
        
        _gameInterface->setGoldPointsLabel(score.coins);
        _gameInterface->setKillPointsLabel(score.kills);
        _gameInterface->setDamagePointsLabel(hero->getDamage());
        
        int totalHealth = (int)GameInfo::getInstance().getFloat("HERO_HEALTH_POINTS");
        int currentHealth = (int)hero->getHealth();
        int percentHealth = (currentHealth * 100) / totalHealth;
        
        _gameInterface->setHealthPointsLabel(percentHealth);
        
        float maxStaminaPoints = GameInfo::getInstance().getFloat("HERO_STAMINA_POINTS");
        float curStaminaPoints = hero->getStamina();
        
        _gameInterface->setStaminaPoints(curStaminaPoints / maxStaminaPoints);
    }
}

void FieldLayer::acceptEvent(const Event &event)
{
    if (event.is("SectorAdded")) {
        Uid uid = event.variables.getInt("uid");
        FieldSector::Ptr sector = _field.getSectorByUid(uid);
        if (sector) {
            FieldSectorWidget *widget = FieldSectorWidget::create(sector);
            widget->setPositionX(sector->getX());
            widget->setPositionY(sector->getY());
            _fieldScroller->addChild(widget, 0, uid);
        }
    } else if (event.is("SectorDeleted")) {
        Uid uid = event.variables.getInt("uid");
        _fieldScroller->removeChildByTag(uid, true);
    } else if (event.is("EntityAdded")) {
        Uid uid = event.variables.getInt("uid");
        Entity *entity = _field.getEntityByUid(uid);
        if (entity) {
            Entity::Type type = entity->getType();
            if (type == Entity::Type::OBSTACLE) {
                auto obstacle = dynamic_cast<Obstacle *>(entity);
                auto widget = ObstacleWidget::create(obstacle);
                _fieldScroller->addChild(widget, 1, uid);
            } else if (type == Entity::Type::ENEMY) {
                auto enemy = dynamic_cast<Enemy *>(entity);
                auto widget = EnemyWidget::create(enemy, _effectsOnField);
                _fieldScroller->addChild(widget, 2, uid);
            } else if (type == Entity::Type::PROJECTILE) {
                auto proj = dynamic_cast<Projectile *>(entity);
                auto widget = ProjectileWidget::create(proj);
                _fieldScroller->addChild(widget, 10, uid);
            }
        } else {
            CC_ASSERT(false);
        }
    } else if (event.is("EntityDeleted")) {
        Uid uid = event.variables.getInt("uid");
        _fieldScroller->removeChildByTag(uid, true);
    } else if (event.is("HeroKilled")) {
        Hero *hero = _field.getHero();
        
        SessionInfo &session = SessionInfo::getInstance();
        SessionInfo::Score score = hero->getScore();
        session.addCoins(score.coins);
        if (session.isBestScore(score)) {
            session.setBestScore(score);
        }
        
        misc::luaSetGlobalInteger("PlayerTotalGoldPoints", session.getCoins());
        misc::luaSetGlobalInteger("PlayerTotalDamagePoints", (int)hero->getWeapon()->damage);
        misc::luaSetGlobalInteger("PlayerBestResultGoldPoints", session.getBestScore().coins);
        misc::luaSetGlobalInteger("PlayerBestResultKillPoints", session.getBestScore().kills);
        misc::luaSetGlobalInteger("PlayerResultGoldPoints", score.coins);
        misc::luaSetGlobalInteger("PlayerResultKillPoints", score.kills);
        auto scene = misc::makeSceneFromLua("CreateResultScene");
        
        cocos2d::Director *director;
        director = cocos2d::Director::getInstance();
        director->replaceScene(scene);
        
        session.save();
    }
}

void FieldLayer::accepter(const Event &event, void *param)
{
    static_cast<FieldLayer *>(param)->acceptEvent(event);
}
