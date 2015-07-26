//
//  EnemyWidget.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 12/07/15.
//
//

#include "EnemyWidget.h"

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
        
        addChild(_bar);
        scheduleUpdate();
        
        return true;
    }
    
    void update(float dt)
    {
        float s = _bar->getScaleX();
        setPositionX(-(s * 0.5f));
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

EnemyWidget* EnemyWidget::create(Enemy *enemy)
{
    EnemyWidget *widget = new EnemyWidget(enemy);
    if (widget && widget->init()) {
        widget->autorelease();
    } else {
        delete widget;
        widget = nullptr;
    }
    return widget;
}

EnemyWidget::EnemyWidget(Enemy *enemy)
: _enemy(enemy)
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
    
    _sprite = cocos2d::Sprite::create(_enemy->getSpriteFilename());
    
    _blood = cocos2d::Sprite::create("effects/blood_sprite.png");
    _blood->setScale(2.0f);
    _blood->setOpacity(0);
    _blood->setVisible(false);
    
    float healthBarYShift = 80.0f;
    _healhBar = HealthBar::create(_enemy->getHealth());
    _healhBar->refresh(_enemy->getHealth());
    _healhBar->setPositionY(healthBarYShift);
    
    float zoneWidth = _enemy->getMelleAreaWidth();
    float zoneHeight = _enemy->getMelleAreaHeight();
    float zoneXAngle = GameInfo::getInstance().getFloat("CAMERA_VIEW_ANGLE");
    
    _hitZoneWidget = MelleHitZone::create(cocos2d::Size(zoneWidth, zoneHeight));
    _hitZoneWidget->setOpacity(0);
    _hitZoneWidget->setVisible(false);
    _hitZoneWidget->setRotation3D(cocos2d::Vec3(-zoneXAngle, 0.0f, 0.0f));
    _hitZoneWidget->setPositionZ(-_sprite->getTexture()->getContentSize().height * 0.5f);
    
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
    setPositionZ(_sprite->getTexture()->getContentSize().height * 0.5f);
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
    
    cocos2d::ScaleTo *scale0 = cocos2d::ScaleTo::create(0.1f, 0.9f);
    cocos2d::ScaleTo *scale1 = cocos2d::ScaleTo::create(0.1f, 1.0f);
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
    }
}

void EnemyWidget::accepter(const Event &event, void *param)
{
    static_cast<EnemyWidget *>(param)->acceptEvent(event);
}
