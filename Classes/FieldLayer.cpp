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
    
    auto gameinfo = GameInfo::getInstance();
    auto director = cocos2d::Director::getInstance();
    auto winSize = director->getWinSize();
    
    _cameraYOffset = gameinfo.getFloat("CAMERA_Y_OFFSET", -850.0f);
    float viewAngle = gameinfo.getFloat("CAMERA_VIEW_ANGLE", 50);
    float viewHeight = gameinfo.getFloat("CAMERA_VIEW_HEIGHT", 1100.0f);
    float squareSize = gameinfo.getFloat("SQUARE_SIZE");
    float viewPosX = squareSize * 3.0f * 0.5f;
    float aspect = winSize.width / winSize.height;
    float fov = gameinfo.getFloat("CAMERA_FIELD_OF_VIEW", 50.0f);
    float zfar = gameinfo.getFloat("CAMERA_FAR_Z_CLIP_PLANE", 1000.0f);
    
    _fieldCamera = cocos2d::Camera::createPerspective(fov, aspect, 0.1f, zfar);
    _fieldCamera->setCameraFlag(cocos2d::CameraFlag::USER1);
    _fieldCamera->setPosition3D(cocos2d::Vec3(viewPosX, 0.0f, viewHeight));
    _fieldCamera->setRotation3D(cocos2d::Vec3(viewAngle, 0.0f, 0.0f));
    
    _fieldScroller = cocos2d::Node::create();
    _fieldScroller->setPosition3D(cocos2d::Vec3(0.0f, 0.0f, 0.0f));
    _fieldScroller->setCameraMask((unsigned short)cocos2d::CameraFlag::USER1);
    _fieldScroller->addChild(_fieldCamera);
    
    _field.setupAccepter(accepter, static_cast<void *>(this));
    _field.initialize();
    
    _heroWidget = HeroWidget::create(_field.getHero());
    _heroWidget->setCameraMask((unsigned short)cocos2d::CameraFlag::USER1);
    
    auto dispatcher = cocos2d::Director::getInstance()->getEventDispatcher();
    
    _controlTouch = ControlTouch::create(_field.getHero(), dispatcher, this);
    _controlKeyboard = ControlKeyboard::create(_field.getHero(), dispatcher, this);
    
    _fieldScroller->addChild(_heroWidget);
    
    addChild(_fieldScroller);
    scheduleUpdate();
    
    return true;
}

void FieldLayer::update(float dt)
{
    _field.idleUpdate(dt);
    
    _fieldCamera->setPositionY(_heroWidget->getPositionY() + _cameraYOffset);
    
    for (auto it = _enemiesWidgets.begin(); it != _enemiesWidgets.end(); ) {
        EnemyWidget *widget = (*it);
        if (widget && widget->isDeletionAllowed()) {
            _fieldScroller->removeChild(widget);
            it = _enemiesWidgets.erase(it);
        } else {
            ++it;
        }
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
        
        int currentHealth = (int)hero->getHealth();
        
        _gameInterface->setHealthPointsLabel(currentHealth);
        
        float maxStaminaPoints = GameInfo::getInstance().getFloat("HERO_STAMINA_POINTS");
        float curStaminaPoints = hero->getStamina();
        
        _gameInterface->setStaminaPoints(curStaminaPoints / maxStaminaPoints);
        
        int passedSectors = _field.getPassedSectors();
        auto difficult = GameInfo::getInstance().getDifficultForSector(passedSectors);
        std::string difficultText = cocos2d::StringUtils::format("%s", difficult.id.c_str());
        _gameInterface->setDifficultLable(difficultText);
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
            widget->setCameraMask((unsigned short)cocos2d::CameraFlag::USER1);
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
                widget->setCameraMask((unsigned short)cocos2d::CameraFlag::USER1);
                _fieldScroller->addChild(widget, 1, uid);
            } else if (type == Entity::Type::ENEMY) {
                auto enemy = dynamic_cast<Enemy *>(entity);
                auto widget = EnemyWidget::create(enemy);
                widget->setCameraMask((unsigned short)cocos2d::CameraFlag::USER1);
                _fieldScroller->addChild(widget, 2, uid);
                _enemiesWidgets.push_back(widget);
            } else if (type == Entity::Type::PROJECTILE) {
                auto proj = dynamic_cast<Projectile *>(entity);
                auto widget = ProjectileWidget::create(proj);
                widget->setCameraMask((unsigned short)cocos2d::CameraFlag::USER1);
                _fieldScroller->addChild(widget, 10, uid);
            }
        } else {
            CC_ASSERT(false);
        }
    } else if (event.is("EntityDeleted")) {
        Uid uid = event.variables.getInt("uid");
        auto entity = _field.getEntityByUid(uid);
        if (entity && !entity->isType(Entity::Type::ENEMY)) {
            _fieldScroller->removeChildByTag(uid, true);
        }
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
