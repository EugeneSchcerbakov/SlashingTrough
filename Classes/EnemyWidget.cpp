//
//  EnemyWidget.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 12/07/15.
//
//

#include "EnemyWidget.h"
#include "EffectsLayer.h"

#include "cocos-ext.h"

// HealthBar implementation

class HealthBar : public cocos2d::Node
{
public:
    static HealthBar* create(int maxHp)
    {
        HealthBar *widget = new HealthBar(maxHp);
        if (widget && widget->init()) {
            widget->autorelease();
        } else {
            delete widget;
            widget = nullptr;
        }
        return widget;
    }
    
    static const float MAX_WIDTH_PX;
    
    void refresh(int healthPoints)
    {
        _curHealthPoints = healthPoints;
        
        float coeff = (float)_curHealthPoints / (float)_maxHealthPoints;
        float width = MAX_WIDTH_PX * coeff;
        
        auto *scale = cocos2d::ScaleTo::create(0.15f, width, 1.0f);
        auto *ease = cocos2d::EaseSineInOut::create(scale);
        _bar->runAction(ease);
    }
    
protected:
    HealthBar(int maxHp)
    : _curHealthPoints(maxHp)
    , _maxHealthPoints(maxHp)
    {
    }
    
    virtual ~HealthBar()
    {
    }
    
    bool init()
    {
        if (!cocos2d::Node::init()) {
            return false;
        }
        
        const float barHeightPx = 16.0f;
        
        _bar = cocos2d::DrawNode::create();
        _bar->drawSolidRect(cocos2d::Vec2::ZERO, cocos2d::Vec2(1.0f, barHeightPx), cocos2d::Color4F::RED);
        
        float coeff = (float)_curHealthPoints / (float)_maxHealthPoints;
        float width = MAX_WIDTH_PX * coeff;
        _bar->setScaleX(width);
        
        addChild(_bar);
        scheduleUpdate();
        
        return true;
    }
    
    void update(float dt)
    {
        float s = _bar->getScaleX();
        setPositionX(-(s * 0.5f));
        
        bool flag = _curHealthPoints < _maxHealthPoints && _curHealthPoints > 0;
        setVisible(flag);
    }
    
private:
    cocos2d::DrawNode *_bar;
    
    int _curHealthPoints;
    const int _maxHealthPoints;
};

const float HealthBar::MAX_WIDTH_PX = 180.0f;

// MelleHitZone implementation

class MelleHitZone : public cocos2d::Node
{
public:
    static MelleHitZone* create(const cocos2d::Size &size)
    {
        MelleHitZone *zone = new MelleHitZone(size);
        if (zone && zone->init()) {
            zone->autorelease();
        } else {
            delete zone;
            zone = nullptr;
        }
        return zone;
    }
    
    void setOpacity(GLubyte opacity) override
    {
        Node::setOpacity(opacity);
        
        float transparency = (float)opacity / 255.0f;
        redrawArea(transparency);
    }
    
protected:
    MelleHitZone(const cocos2d::Size &size)
    : _size(size)
    , _zoneOpacity(0.7f)
    {
    }
    
    virtual ~MelleHitZone()
    {
    }
    
    bool init() override
    {
        if (!cocos2d::Node::init()) {
            return false;
        }
        
        _zone = cocos2d::DrawNode::create();
        
        addChild(_zone, 0);
        setContentSize(_size);
        
        redrawArea(1.0f);
        
        return true;
    }
    
private:
    void redrawArea(float opacity)
    {
        cocos2d::Color4F zoneColor(1.0f, 0.0f, 1.0f, _zoneOpacity * opacity);
        
        cocos2d::Vec2 origin(-_size.width * 0.5f, -_size.height * 0.5f);
        cocos2d::Vec2 destin(_size.width * 0.5f, _size.height * 0.5f);
        
        // draw zone
        _zone->clear();
        _zone->drawSolidRect(origin, destin, zoneColor);
    }
    
    cocos2d::DrawNode *_zone;
    const cocos2d::Size _size;
    const float _zoneOpacity;
};

// EnemyWidget implementation

EnemyWidget* EnemyWidget::create(Enemy *enemy, cocos2d::Layer *fieldEffects)
{
    EnemyWidget *widget = new EnemyWidget(enemy, fieldEffects);
    if (widget && widget->init()) {
        widget->autorelease();
    } else {
        delete widget;
        widget = nullptr;
    }
    return widget;
}

EnemyWidget::EnemyWidget(Enemy *enemy, cocos2d::Layer *fieldEffects)
: _enemy(enemy)
, _fieldEffects(fieldEffects)
, _allowDeletion(false)
{
    CC_ASSERT(_enemy);
}

EnemyWidget::~EnemyWidget()
{
}

bool EnemyWidget::isDeletionAllowed() const
{
    return _allowDeletion;
}

bool EnemyWidget::init()
{
    if (!cocos2d::BillBoard::init()) {
        return false;
    }
    
    _enemy->setupAccepter(accepter, static_cast<void *>(this));
    
    float spriteHeght = 0.0f;
    float healthBarYShift = 80.0f;
    
    std::string modelFilename = _enemy->getModelFilename();
    if (!modelFilename.empty())
    {
        _scale = 10.0f;
        healthBarYShift = 140.0;
        
        _sprite = cocos2d::Sprite3D::create(modelFilename);
        _sprite->setScale(_scale);
        _sprite->setRotation3D(cocos2d::Vec3(-45.0f, 180.0f, 0.0f));
        
        cocos2d::Color3B color;
        color.r = (GLubyte)_enemy->getColorR();
        color.g = (GLubyte)_enemy->getColorG();
        color.b = (GLubyte)_enemy->getColorB();
        _sprite->setColor(color);
        
        auto animation = cocos2d::Animation3D::create(modelFilename);
        auto state = cocos2d::Animate3D::create(animation);
        
        float anim_speed = state->getSpeed() * 0.5 * CCRANDOM_0_1();
        bool reverse = CCRANDOM_0_1() > 0.5f;
        
        state->setSpeed(reverse ? -anim_speed : anim_speed);
        state->setQuality(cocos2d::Animate3DQuality::QUALITY_HIGH);
        auto animate = cocos2d::RepeatForever::create(state);
        _sprite->runAction(animate);
    }
    else
    {
        _scale = 1.0f;
        _sprite = cocos2d::Sprite::create(_enemy->getSpriteFilename());
        auto cast = dynamic_cast<cocos2d::Sprite *>(_sprite);
        spriteHeght = cast->getTexture()->getContentSize().height;
    }
    
    _blood = cocos2d::Sprite::create("effects/blood_sprite.png");
    _blood->setScale(2.0f);
    _blood->setOpacity(0);
    _blood->setVisible(false);

    _healhBar = HealthBar::create(_enemy->getHealth());
    _healhBar->refresh(_enemy->getHealth());
    _healhBar->setPositionY(healthBarYShift);
    
    float zoneWidth = _enemy->getMelleAreaWidth();
    float zoneHeight = _enemy->getMelleAreaHeight();
    float zoneXAngle = GameInfo::getInstance().getConstFloat("CAMERA_VIEW_ANGLE");
    
    _hitZoneWidget = MelleHitZone::create(cocos2d::Size(zoneWidth, zoneHeight));
    _hitZoneWidget->setOpacity(0);
    _hitZoneWidget->setVisible(false);
    _hitZoneWidget->setRotation3D(cocos2d::Vec3(-zoneXAngle, 0.0f, 0.0f));
    
    _weapon = cocos2d::Sprite::create("gamefield/wpn_iron_sword.png");
    _weapon->setAnchorPoint(cocos2d::Vec2(0.5f, 0.0f));
    _weapon->setVisible(false);
    
    addChild(_hitZoneWidget, 0);
    addChild(_weapon, 1);
    addChild(_sprite, 2);
    addChild(_blood, 3);
    addChild(_healhBar, 4);
    scheduleUpdate();
    setPositionX(_enemy->getPositionX());
    setPositionY(_enemy->getPositionY());
    setPositionZ(spriteHeght * 0.5f);
    setMode(cocos2d::BillBoard::Mode::VIEW_PLANE_ORIENTED);
    
    return true;
}

void EnemyWidget::update(float dt)
{
    
}

void EnemyWidget::runHitAccentEffect()
{
    auto start = [&](){_blood->setVisible(true);};
    auto finish = [&](){_blood->setVisible(false);};
    cocos2d::FadeIn *fadein = cocos2d::FadeIn::create(0.2f);
    cocos2d::FadeOut *fadeout = cocos2d::FadeOut::create(1.0f);
    cocos2d::CallFunc *start_func = cocos2d::CallFunc::create(start);
    cocos2d::CallFunc *finish_func = cocos2d::CallFunc::create(finish);
    cocos2d::Sequence *bloodEffect = cocos2d::Sequence::create(start_func, fadein, fadeout, finish_func, nullptr);
    _blood->runAction(bloodEffect);
    
    cocos2d::ScaleTo *scale0 = cocos2d::ScaleTo::create(0.1f, _scale * 0.9f);
    cocos2d::ScaleTo *scale1 = cocos2d::ScaleTo::create(0.1f, _scale);
    cocos2d::Sequence *effect = cocos2d::Sequence::create(scale0, scale1, nullptr);
    _sprite->runAction(effect);
}

void EnemyWidget::acceptEvent(const Event &event)
{
    if (event.is("DamageReceived")) {
        int health = (int)_enemy->getHealth();
        _healhBar->refresh(health);
        if (_enemy->isAlive()) {
            runHitAccentEffect();
        } else {
            auto func = [this](){_allowDeletion = true;};
            auto fade = cocos2d::FadeOut::create(0.2f);
            auto call = cocos2d::CallFunc::create(func);
            auto effect = cocos2d::Sequence::create(fade, call, nullptr);
            _sprite->runAction(effect);
        }
    } else if (event.is("FatalDamageReceived")) {
        _allowDeletion = true;
    } else if (event.is("ShowMelleHighlight")) {
        float showTime = event.variables.getFloat("ShowTime");
        float zoneX = _enemy->getMelleAreaCenterX() - _enemy->getPositionX();
        float zoneY = _enemy->getMelleAreaCenterY() - _enemy->getPositionY();

        auto fadein = cocos2d::FadeIn::create(showTime);
        
        _hitZoneWidget->setVisible(true);
        _hitZoneWidget->setPosition(zoneX, zoneY);
        _hitZoneWidget->runAction(fadein);
    } else if (event.is("ShowMelleAttack")) {
        float showTime = event.variables.getFloat("ShowTime");
        float sideCoeff = 0.0f;
        if (_enemy->isMelleType(GameInfo::EnemyType::Melle::SAME_LINE)) {
            sideCoeff = 4.0f;
        } else if (_enemy->isMelleType(GameInfo::EnemyType::Melle::LEFT_LINE)) {
            sideCoeff = 1.0f;
        } else if (_enemy->isMelleType(GameInfo::EnemyType::Melle::RIGHT_LINE)) {
            sideCoeff = 7.0f;
        }
        
        auto func_end = [&](){_weapon->setVisible(false);};
        auto end_call = cocos2d::CallFunc::create(func_end);
        auto fadein = cocos2d::FadeIn::create(showTime * 0.5f);
        auto fadeout = cocos2d::FadeOut::create(showTime * 0.5f);
        auto rotate = cocos2d::RotateBy::create(showTime, -120.0f);
        auto rotate_ease = cocos2d::EaseSineInOut::create(rotate);
        auto transparency = cocos2d::Sequence::create(fadein, fadeout, nullptr);
        auto motion = cocos2d::Spawn::create(transparency, rotate_ease, nullptr);
        auto action = cocos2d::Sequence::create(motion, end_call, nullptr);
        
        if (_hitZoneWidget->isVisible()) {
            auto zone_end = [&](){_hitZoneWidget->setVisible(false);};
            auto zone_hide = cocos2d::FadeOut::create(0.25f);
            auto zone_wait = cocos2d::DelayTime::create(showTime);
            auto zone_call = cocos2d::CallFunc::create(zone_end);
            auto zone_action = cocos2d::Sequence::create(zone_wait, zone_hide, zone_call, nullptr);
            _hitZoneWidget->runAction(zone_action);
        }
        
        _weapon->setVisible(true);
        _weapon->setOpacity(0);
        _weapon->setRotation(-30.0f * sideCoeff);
        _weapon->runAction(action);
	} else if (event.is("HitedByProjectile")) {
		cocos2d::Vec3 offset = cocos2d::Vec3(0.0f, 0.0f, 50.0f);
		auto particle = cocos2d::PUParticleSystem3D::create("particles/explosion.pu", "particles/explosion.material");
		particle->setScale(10.0f);
		particle->setPosition3D(getPosition3D() + offset);
		particle->setCameraMask(Effect::TargetColor);
		particle->startParticleSystem();
		_fieldEffects->addChild(particle);
        
        Effect *distortion = Effect::create("effects/circle_distortion.png");
        distortion->setPosition3D(getPosition3D());
        distortion->setCameraMask(Effect::TargetDistor);
        distortion->setScale(0.3f);
        distortion->setOpacity(0);
        auto finish = [distortion](){distortion->finish();};
        auto callback = cocos2d::CallFunc::create(finish);
        auto fadein = cocos2d::FadeIn::create(0.1f);
        auto fadeout = cocos2d::FadeOut::create(0.3f);
        auto fadestay = cocos2d::DelayTime::create(0.2f);
        auto fade = cocos2d::Sequence::create(fadein, fadestay, fadeout, nullptr);
        auto scale = cocos2d::ScaleTo::create(1.0f, 2.0f);
        auto effect = cocos2d::Spawn::create(fade, scale, nullptr);
        auto delay = cocos2d::DelayTime::create(0.2f);
        auto seq = cocos2d::Sequence::create(delay, effect, callback, nullptr);
        distortion->runAction(seq);
        _fieldEffects->addChild(distortion);
	}
}

void EnemyWidget::accepter(const Event &event, void *param)
{
    static_cast<EnemyWidget *>(param)->acceptEvent(event);
}
