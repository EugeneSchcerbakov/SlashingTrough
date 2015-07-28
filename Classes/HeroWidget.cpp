//
//  HeroWidget.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 11/07/15.
//
//

#include "HeroWidget.h"
#include "Store.h"

const SwordTrans HeroWidget::_swordRightTrans(cocos2d::Vec2(35.0f, 0.0f), 160.0f);
const SwordTrans HeroWidget::_swordLeftTrans(cocos2d::Vec2(-35.0f, 0.0f), 160.0f);

HeroWidget* HeroWidget::create(Hero *hero)
{
    HeroWidget *widget = new HeroWidget(hero);
    if (widget && widget->init()) {
        widget->autorelease();
    } else {
        delete widget;
        widget = nullptr;
    }
    return widget;
}

HeroWidget::HeroWidget(Hero *hero)
: _hero(hero)
{
}

HeroWidget::~HeroWidget()
{
}

bool HeroWidget::init()
{
    if (!cocos2d::BillBoard::init()) {
        return false;
    }
    
    _hero->setupAccepter(accepter, static_cast<void *>(this));
    EquipWeapon *weapon = _hero->getWeapon();
    float trailLen = 0.0f, trailWidth = 0.0f;
    std::string trailTex, swordTex;
    if (weapon) {
        trailLen = weapon->trail.length;
        trailWidth = weapon->trail.width;
        trailTex = weapon->trail.texture;
        swordTex = weapon->sprite;
    } else {
        CC_ASSERT(weapon);
        // trying to stay stable
        Equip::WeakPtr default_ptr = Store::getInstance().getItemById(Store::DEFAULT_WEAPON_ID);
        EquipWeapon *default_wpn = EquipWeapon::cast(default_ptr.lock());
        trailLen = default_wpn->trail.length;
        trailWidth = default_wpn->trail.width;
        trailTex = default_wpn->trail.texture;
        swordTex = default_wpn->sprite;
    }
    
    _bodyController = cocos2d::Node::create();
    _body = cocos2d::Sprite::create("gamefield/hero.png");
    _sword = cocos2d::Sprite::create();
    _sword->setTexture(swordTex);
    _sword->setAnchorPoint(cocos2d::Vec2(0.5f, 0.0f));
    _sword->setPosition(_swordRightTrans.pos);
    _sword->setRotation(_swordRightTrans.angle);
    _sword->setScale(0.7f);
    
    _bodyController->addChild(_body, 0);
    _bodyController->addChild(_sword, 1);
    
    _swordTrail = cocos2d::MotionStreak::create(trailLen, 1.0f, trailWidth, cocos2d::Color3B::WHITE, trailTex);
    _swordTrail->setOpacity(0);
    
    _swordSide = SwordSide::RIGHT;
	_nextSwordSide = _swordSide;
    
    setPositionZ(_body->getTexture()->getContentSize().height * 0.5f);
    setMode(cocos2d::BillBoard::Mode::VIEW_PLANE_ORIENTED);
    addChild(_bodyController, 0);
    addChild(_swordTrail, 1);
    scheduleUpdate();
    
    return true;
}

void HeroWidget::update(float dt)
{
    setPositionX(_hero->getPositionX());
    setPositionY(_hero->getPositionY());
    
    if (_swordTrail->isVisible())
    {
        float posXCoeff = 0.5f;
        float posYCoeff = 1.0f;
        EquipWeapon *weapon = _hero->getWeapon();
        if (weapon) {
            posYCoeff = weapon->trail.posYCoeff;
        } else {
            CC_ASSERT(false);
            posYCoeff = 0.0f;
        }
        
        float x = _sword->getContentSize().width * posXCoeff;
        float y = _sword->getContentSize().height * posYCoeff;
        
        cocos2d::Vec2 local = cocos2d::Vec2(x, y);
        cocos2d::Vec2 world = _sword->convertToWorldSpace(local);
        
        _swordTrail->setPosition(convertToNodeSpace(world));
    }
}

void HeroWidget::removeAllAnimations()
{
	auto actionMgr = cocos2d::Director::getInstance()->getActionManager();
	if (actionMgr->getNumberOfRunningActionsInTarget(_bodyController) > 0){
		actionMgr->removeAllActionsFromTarget(_bodyController);
	}
	if (actionMgr->getNumberOfRunningActionsInTarget(_sword) > 0) {
		actionMgr->removeAllActionsFromTarget(_sword);
	}
}

void HeroWidget::runSwordTrailEffect(float duration)
{
    auto func_start = [&](){_swordTrail->setVisible(true);};
    auto func_end = [&](){_swordTrail->setVisible(true);};
    
    float opacityCoeff = 1.0f;
    EquipWeapon *weapon = _hero->getWeapon();
    if (weapon) {
        opacityCoeff = weapon->trail.opacity;
    } else {
        CC_ASSERT(false);
    }
    
    float fadeinTime = duration * 0.6f;
    float fadeoutTime = duration * 0.3f;
    float delayTime = duration * 0.1f;
    GLubyte opacity = (GLubyte)(255.0f * opacityCoeff);
    
    cocos2d::CallFunc *start = cocos2d::CallFunc::create(func_start);
    cocos2d::CallFunc *end = cocos2d::CallFunc::create(func_end);
    cocos2d::DelayTime *dealy = cocos2d::DelayTime::create(delayTime);
    cocos2d::FadeTo *fadein = cocos2d::FadeTo::create(fadeinTime, opacity);
    cocos2d::FadeOut *fadeout = cocos2d::FadeOut::create(fadeoutTime);
    cocos2d::Sequence *effect = cocos2d::Sequence::create(start, fadein, dealy, fadeout, end, nullptr);
    
    _swordTrail->reset();
    _swordTrail->runAction(effect);
}

void HeroWidget::acceptEvent(const Event &event)
{
	if (event.is("SwipeRight")) {
		removeAllAnimations();
		if (_swordSide != _nextSwordSide) {
			_swordSide = _nextSwordSide;
		}
		float time = event.variables.getFloat("duration");
        if (_swordSide == SwordSide::RIGHT) {
			auto bodyAction = AnimBodySwipeRight(time);
			auto swordAction = AnimSwordRightSwipeRight(time);
			_sword->runAction(swordAction);
			_bodyController->runAction(bodyAction);
			_nextSwordSide = SwordSide::RIGHT;
        } else if (_swordSide == SwordSide::LEFT) {
			auto swordAction = AnimSwordLeftSwipeRight(time);
			_sword->runAction(swordAction);
			_nextSwordSide = SwordSide::RIGHT;
        }
		runSwordTrailEffect(time);
    } else if (event.is("SwipeLeft")) {
		removeAllAnimations();
		if (_swordSide != _nextSwordSide) {
			_swordSide = _nextSwordSide;
		}
		float time = event.variables.getFloat("duration");
        if (_swordSide == SwordSide::RIGHT) {
			auto swordAction = AnimSwordRightSwipeLeft(time);
			_sword->runAction(swordAction);
			_nextSwordSide = SwordSide::LEFT;
        } else if (_swordSide == SwordSide::LEFT) {
			auto bodyAction = AnimBodySwipeLeft(time);
			auto swordAction = AnimSwordLeftSwipeLeft(time);
			_sword->runAction(swordAction);
			_bodyController->runAction(bodyAction);
			_nextSwordSide = SwordSide::LEFT;
        }
		runSwordTrailEffect(time);
    } else if (event.is("JumpBack")) {

    } else if (event.is("DamageReceived")) {
        auto tint0 = cocos2d::TintTo::create(0.1f, cocos2d::Color3B::RED);
        auto tint1 = cocos2d::TintTo::create(0.1f, cocos2d::Color3B::WHITE);
        auto effect = cocos2d::Sequence::create(tint0, tint1, nullptr);
        _body->runAction(effect);
    } else {
        CC_ASSERT(false);
    }
}

void HeroWidget::accepter(const Event &event, void *param)
{
    static_cast<HeroWidget *>(param)->acceptEvent(event);
}

cocos2d::FiniteTimeAction* HeroWidget::AnimSwordRightSwipeRight(float duration)
{
    // sword anim
	auto sword_rotate0 = cocos2d::RotateBy::create(duration*0.5f, -60.0f);
	auto sword_rotate1 = cocos2d::RotateBy::create(duration*0.5f, 60.0f);
	auto sword_rotate = cocos2d::Sequence::create(sword_rotate0, sword_rotate1, nullptr);
    return sword_rotate;
}

cocos2d::FiniteTimeAction* HeroWidget::AnimSwordRightSwipeLeft(float duration)
{
    // sword anim
	auto sword_move = cocos2d::MoveTo::create(duration, _swordLeftTrans.pos);
	auto sword_rotate = cocos2d::RotateBy::create(duration, -320.0f);
	auto sword_move_ease = cocos2d::EaseSineInOut::create(sword_move);
	auto sword_rotate_ease = cocos2d::EaseSineInOut::create(sword_rotate);
	auto sword_anim = cocos2d::Spawn::create(sword_move_ease, sword_rotate_ease, nullptr);
    return sword_anim;
}

cocos2d::FiniteTimeAction* HeroWidget::AnimSwordLeftSwipeRight(float duration)
{
    // sword anim
	auto sword_move = cocos2d::MoveTo::create(duration, _swordRightTrans.pos);
	auto sword_rotate = cocos2d::RotateBy::create(duration, 320.0f);
	auto sword_move_ease = cocos2d::EaseSineInOut::create(sword_move);
	auto sword_rotate_ease = cocos2d::EaseSineInOut::create(sword_rotate);
	auto sword_anim = cocos2d::Spawn::create(sword_move_ease, sword_rotate_ease, nullptr);
    return sword_anim;
}

cocos2d::FiniteTimeAction* HeroWidget::AnimSwordLeftSwipeLeft(float duration)
{
	auto sword_rotate0 = cocos2d::RotateBy::create(duration*0.5f, 60.0f);
	auto sword_rotate1 = cocos2d::RotateBy::create(duration*0.5f, -60.0f);
	auto sword_rotate = cocos2d::Sequence::create(sword_rotate0, sword_rotate1, nullptr);
    return sword_rotate;
}

cocos2d::FiniteTimeAction* HeroWidget::AnimBodySwipeRight(float duration)
{
	auto rotate = cocos2d::RotateBy::create(duration, -360.0f);
	auto rotate_ease = cocos2d::EaseSineInOut::create(rotate);
	auto anim = cocos2d::Sequence::create(rotate_ease, nullptr);
	return anim;
}

cocos2d::FiniteTimeAction* HeroWidget::AnimBodySwipeLeft(float duration)
{
	auto rotate = cocos2d::RotateBy::create(duration, 360.0f);
	auto rotate_ease = cocos2d::EaseSineInOut::create(rotate);
	auto anim = cocos2d::Sequence::create(rotate_ease, nullptr);
	return anim;
}