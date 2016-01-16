//
//  FieldLayer.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 11/07/15.
//
//

#include "FieldLayer.h"
#include "GameInfo.h"
#include "PlayerInfo.h"
#include "Utils.h"
#include "LevelsCache.h"
#include "ScreenChanger.h"
#include "Log.h"

FieldLayer* FieldLayer::create(const std::string &levelId, GameInterface *gameInterface)
{
    FieldLayer *layer = new FieldLayer(gameInterface);
    if (layer && layer->init(levelId)) {
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

bool FieldLayer::init(const std::string &levelId)
{
    if (!cocos2d::Layer::init()) {
        return false;
    }
    
    auto director = cocos2d::Director::getInstance();
    
    cocos2d::Size fboSize = director->getWinSizeInPixels();
    
    std::string distortionSource = cocos2d::FileUtils::getInstance()->getStringFromFile("shaders/distortionPost.fsh");
    _distortionShader = cocos2d::GLProgram::createWithByteArrays(cocos2d::ccPositionTexture_vert, distortionSource.c_str());
    _distortionShader->bindAttribLocation(cocos2d::GLProgram::ATTRIBUTE_NAME_POSITION, cocos2d::GLProgram::VERTEX_ATTRIB_POSITION);
    _distortionShader->bindAttribLocation(cocos2d::GLProgram::ATTRIBUTE_NAME_TEX_COORD, cocos2d::GLProgram::VERTEX_ATTRIB_TEX_COORDS);
    _distortionShader->link();
    _distortionShader->updateUniforms();
    _distortionShader->retain();
    
    _distortionState = cocos2d::GLProgramState::getOrCreateWithGLProgram(_distortionShader);
    _distortionState->retain();
    
    _rtSceneColor = cocos2d::experimental::RenderTarget::create(fboSize.width, fboSize.height);
    _rtSceneDepth = cocos2d::experimental::RenderTargetDepthStencil::create(fboSize.width, fboSize.height);
    _rtSceneDistor = cocos2d::experimental::RenderTarget::create(fboSize.width, fboSize.height);
    
    _distortionState->setUniformTexture("normalBuffer", _rtSceneDistor->getTexture());
    
    _fboColor = cocos2d::experimental::FrameBuffer::create(1, fboSize.width, fboSize.height);
    _fboColor->attachRenderTarget(_rtSceneColor);
    _fboColor->attachDepthStencilTarget(_rtSceneDepth);
    _fboColor->setClearColor(cocos2d::Color4F(0.0f, 0.0f, 0.0f, 1.0f));
    _fboColor->setClearDepth(0.0f);
    
    _fboDistor = cocos2d::experimental::FrameBuffer::create(1, fboSize.width, fboSize.height);
    _fboDistor->attachRenderTarget(_rtSceneDistor);
    _fboDistor->setClearColor(cocos2d::Color4F(0.0f, 0.0f, 0.0f, 1.0f));
    _fboDistor->setClearDepth(0.0f);
    
    _rtColorBuffer = cocos2d::Sprite::create();
    _rtColorBuffer->initWithTexture(_rtSceneColor->getTexture());
    _rtColorBuffer->setGLProgramState(_distortionState);
    _rtColorBuffer->setPosition(fboSize.width*0.5f, fboSize.height*0.5f);
    _rtColorBuffer->setFlippedY(true);
    
    _fieldEffects = EffectsLayer::create();
    
    _fieldCamera = FieldCamera::create(cocos2d::CameraFlag::USER1);
    _fieldCamera->getActualCamera()->setFrameBufferObject(_fboColor);
    _distorCamera = FieldCamera::create(cocos2d::CameraFlag::USER2);
    _distorCamera->getActualCamera()->setFrameBufferObject(_fboDistor);
    
    _field.setupAccepter(accepter, static_cast<void *>(this));
    _field.initialize(LevelsCache::getInstance().getLevelById(levelId));
    
    _heroWidget = HeroWidget::create(_field.getHero(), _fieldEffects);
    _heroWidget->setCameraMask((unsigned short)cocos2d::CameraFlag::USER1);
    
    auto dispatcher = cocos2d::Director::getInstance()->getEventDispatcher();
    
    _controlTouch = ControlTouch::create(_field.getHero(), dispatcher, this);
    _controlKeyboard = ControlKeyboard::create(_field.getHero(), dispatcher, this);
    
    addChild(_fieldCamera);
    addChild(_distorCamera);
    addChild(_heroWidget);
    addChild(_rtColorBuffer);
    addChild(_fieldEffects);
    scheduleUpdate();
    setPosition3D(cocos2d::Vec3(0.0f, 0.0f, 0.0f));
    
    return true;
}

void FieldLayer::update(float dt)
{
    _field.idleUpdate(dt);
    
    cocos2d::Vec2 camPos;
    camPos.x = _heroWidget->getPositionX();
    camPos.y = _heroWidget->getPositionY();
    _fieldCamera->setTargetPosition(camPos);
    _distorCamera->setTargetPosition(camPos);
    
    for (auto it = _enemiesWidgets.begin(); it != _enemiesWidgets.end(); ) {
        EnemyWidget *widget = (*it);
        if (widget && widget->isDeletionAllowed()) {
            removeChild(widget);
            it = _enemiesWidgets.erase(it);
        } else {
            ++it;
        }
    }
    
    refreshInterface();
}

void FieldLayer::visit(cocos2d::Renderer *renderer, const cocos2d::Mat4 &parentTransform, uint32_t parentFlags)
{
    cocos2d::Layer::visit(renderer, parentTransform, parentFlags);
}

void FieldLayer::refreshInterface()
{
    Hero *hero = _field.getHero();
    
    if (hero)
    {
        PlayerInfo::Score score = hero->getScore();
        
        _gameInterface->setGoldPointsLabel(score.coins);
        _gameInterface->setKillPointsLabel(score.kills);
        _gameInterface->setDamagePointsLabel(hero->getDamage());
        
        float currentHealth = hero->getHealth();
        float maximumHealth = hero->getMaxHealth();
        float percentageHealth = currentHealth / maximumHealth * 100;
        
        _gameInterface->setHealthPointsLabel(percentageHealth);
        
        auto sector = _field.getCurrentSector();
        if (sector) {
            std::string difficultText = cocos2d::StringUtils::format("preset id: %s", sector->getPresetId().c_str());
            _gameInterface->setDifficultLable(difficultText);
        }
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
            widget->setCameraMask(EffectsLayer::TargetColor);
            addChild(widget, 0, uid);
        }
    } else if (event.is("SectorDeleted")) {
        Uid uid = event.variables.getInt("uid");
        removeChildByTag(uid, true);
    } else if (event.is("EntityAdded")) {
        Uid uid = event.variables.getInt("uid");
        Entity *entity = _field.getEntityByUid(uid);
        if (entity) {
            Entity::Type type = entity->getType();
            if (type == Entity::Type::OBSTACLE) {
                auto obstacle = dynamic_cast<Obstacle *>(entity);
                auto widget = ObstacleWidget::create(obstacle);
                widget->setCameraMask(EffectsLayer::TargetColor);
                addChild(widget, 1, uid);
            } else if (type == Entity::Type::ENEMY) {
                auto enemy = dynamic_cast<Enemy *>(entity);
                auto widget = EnemyWidget::create(enemy, _fieldEffects);
                widget->setCameraMask(EffectsLayer::TargetColor);
                addChild(widget, 2, uid);
                _enemiesWidgets.push_back(widget);
            } else if (type == Entity::Type::PROJECTILE) {
                auto proj = dynamic_cast<Projectile *>(entity);
                auto widget = ProjectileWidget::create(proj);
                widget->setCameraMask(EffectsLayer::TargetColor);
                addChild(widget, 10, uid);
            }
        } else {
            CC_ASSERT(false);
        }
    } else if (event.is("EntityDeleted")) {
        Uid uid = event.variables.getInt("uid");
        auto entity = _field.getEntityByUid(uid);
        if (entity && !entity->isType(Entity::Type::ENEMY)) {
            removeChildByTag(uid, true);
        }
    } else if (event.is("LevelFinished")) {
        LevelsCache &levelsCache = LevelsCache::getInstance();
    
        bool victory = event.variables.getBool("victory", false);
        std::string levelId = event.variables.getString("levelId");
        FieldLevel::WeakPtr level_ptr = levelsCache.getLevelById(levelId);
        FieldLevel::Ptr level = level_ptr.lock();
    
        if (level_ptr.expired()) {
            WRITE_WARN("Failed to get result from invalid level: " + levelId);
        }
        
        bool completitionFact = victory && !level->isStatus(FieldLevel::Status::COMPLETED);
        
        if (victory) {
            makeLevelComplete(level_ptr);
        }
            
        ScreenChanger::showStatistics(level_ptr, _field.getHero()->getScore(), victory, completitionFact);
    }
}

void FieldLayer::accepter(const Event &event, void *param)
{
    static_cast<FieldLayer *>(param)->acceptEvent(event);
}

void FieldLayer::makeLevelComplete(FieldLevel::WeakPtr levelPtr)
{
    if (levelPtr.expired()) {
        WRITE_WARN("Failed to complete invalid level");
        return;
    }
    
    FieldLevel::Ptr level = levelPtr.lock();
    if (!level->isStatus(FieldLevel::Status::COMPLETED))
    {
        PlayerInfo &player = PlayerInfo::getInstance();
        LevelsCache &levelsCache = LevelsCache::getInstance();
        
        std::string id = level->getId();
        
        level->setStatus(FieldLevel::Status::COMPLETED);
        player.variables.setString("LastCompletedLevel", id);
        WRITE_LOG("Level completed: " + id);
            
        // unlock levels
        auto unlocks = level->getUnlocks();
        for (auto id : unlocks) {
            FieldLevel::WeakPtr inst_ptr = levelsCache.getLevelById(id);
            if (!inst_ptr.expired()) {
                FieldLevel::Ptr inst = inst_ptr.lock();
                inst->setStatus(FieldLevel::Status::UNLOCKED);
                WRITE_LOG("Level unlocked: " + id);
            } else {
                WRITE_WARN("Failed to unlock null level with id: " + id);
            }
        }
    }
}
