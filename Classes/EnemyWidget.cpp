//
//  EnemyWidget.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 12/07/15.
//
//

#include "EnemyWidget.h"
#include "EffectEnemyDeath.h"

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

// EnemyWidget implementation

EnemyWidget* EnemyWidget::create(Enemy *enemy, EffectsLayer *effects)
{
    EnemyWidget *widget = new EnemyWidget(enemy, effects);
    if (widget && widget->init()) {
        widget->autorelease();
    } else {
        delete widget;
        widget = nullptr;
    }
    return widget;
}

EnemyWidget::EnemyWidget(Enemy *enemy, EffectsLayer *effects)
: _enemy(enemy)
, _effects(effects)
{
    CC_ASSERT(_enemy);
}

EnemyWidget::~EnemyWidget()
{
}

bool EnemyWidget::init()
{
    if (!cocos2d::Node::init()) {
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
    
    addChild(_sprite, 0);
    addChild(_blood, 1);
    addChild(_healhBar, 2);
    scheduleUpdate();
    setPositionX(_enemy->getPositionX());
    setPositionY(_enemy->getPositionY());
    
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
            auto *effect = EffectEnemyDeath::create(_enemy->getSpriteFilename(), getPosition());
            _effects->addEffect(effect);
        }
    }
}

void EnemyWidget::accepter(const Event &event, void *param)
{
    static_cast<EnemyWidget *>(param)->acceptEvent(event);
}
