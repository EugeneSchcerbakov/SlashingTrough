//
//  HeroWidget.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 11/07/15.
//
//

#include "HeroWidget.h"
#include "Store.h"
#include "EffectExplosion.h"

#include "audio/include/SimpleAudioEngine.h"
#include "Utils.h"

const SwordTrans HeroWidget::_swordRightTrans(cocos2d::Vec2(35.0f, 0.0f), 160.0f);
const SwordTrans HeroWidget::_swordLeftTrans(cocos2d::Vec2(-35.0f, 0.0f), 160.0f);

HeroWidget* HeroWidget::create(Hero *hero, cocos2d::Layer *fieldEffects)
{
    HeroWidget *widget = new HeroWidget(hero, fieldEffects);
    if (widget && widget->init()) {
        widget->autorelease();
    } else {
        delete widget;
        widget = nullptr;
    }
    return widget;
}

HeroWidget::HeroWidget(Hero *hero, cocos2d::Layer *fieldEffects)
: _hero(hero)
, _fieldEffects(fieldEffects)
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
    
    ItemWeapon *weapon = _hero->getWeapon();
    float trailLen = 0.0f, trailWidth = 0.0f;
    std::string trailTex, swordTex;
    if (weapon) {
        trailLen = weapon->getTrailInfo().length;
        trailWidth = weapon->getTrailInfo().width;
        trailTex = weapon->getTrailInfo().texture;
        swordTex = weapon->getSprite();
    } else {
        CC_ASSERT(weapon);
        // trying to stay stable
        Item::WeakPtr default_ptr = Store::getInstance().getItemById(PlayerInfo::DEFAULT_WEAPON_ID);
        ItemWeapon *default_wpn = ItemWeapon::cast(default_ptr.lock());
        trailLen = default_wpn->getTrailInfo().length;
        trailWidth = default_wpn->getTrailInfo().width;
        trailTex = default_wpn->getTrailInfo().texture;
        swordTex = default_wpn->getSprite();
    }
    
    _bodyController = cocos2d::Node::create();
    _body = cocos2d::Sprite::create("gamefield/hero.png");
    _sword = cocos2d::Sprite::create();
    _sword->setTexture(swordTex);
    _sword->setAnchorPoint(cocos2d::Vec2(0.5f, 0.0f));
    _sword->setPosition(_swordRightTrans.pos);
    _sword->setRotation(_swordRightTrans.angle);
    _sword->setScale(0.7f);
    
    _shield = cocos2d::Sprite::create("gamefield/shield.png");
    _shield->setPositionY(100.0f);
    _shield->setScale(1.5f);
    _shield->setVisible(false);
    
    _bodyController->addChild(_body, 0);
    _bodyController->addChild(_sword, 1);
    
    _swordTrail = cocos2d::MotionStreak::create(trailLen, 1.0f, trailWidth, cocos2d::Color3B::WHITE, trailTex);
    _swordTrail->setOpacity(0);
    
    _dashDistor = cocos2d::Sprite::create("effects/dash_distortion.png");
    _dashDistor->setCameraMask(Effect::TargetDistor);
    _dashDistor->setScale(5.0f);
    _dashDistor->setVisible(false);
    _fieldEffects->addChild(_dashDistor);
    
    _swordSide = SwordSide::RIGHT;
	_nextSwordSide = _swordSide;
    
    setPositionZ(_body->getTexture()->getContentSize().height * 0.5f);
    setMode(cocos2d::BillBoard::Mode::VIEW_PLANE_ORIENTED);
    addChild(_bodyController, 0);
    addChild(_swordTrail, 1);
    addChild(_shield, 2);
    scheduleUpdate();

    _hero->registerEventHandler("swipe_right", BIND_EVENT_HANDLER(HeroWidget::_handleEventSwipeRight, this));
    _hero->registerEventHandler("swipe_left", BIND_EVENT_HANDLER(HeroWidget::_handleEventSwipeLeft, this));
    _hero->registerEventHandler("jump_back_start", BIND_EVENT_HANDLER(HeroWidget::_handleEventJumpBackStart, this));
    _hero->registerEventHandler("jump_forward_attack", BIND_EVENT_HANDLER(HeroWidget::_handleEventJumpForwardAttack, this));
    _hero->registerEventHandler("hide_shield", BIND_EVENT_HANDLER(HeroWidget::_handleEventHideShield, this));
    _hero->registerEventHandler("shield_damage_received", BIND_EVENT_HANDLER(HeroWidget::_handleEventShieldDamageReceived, this));
    _hero->registerEventHandler("damage_received", BIND_EVENT_HANDLER(HeroWidget::_handleEventDamageReceived, this));
    _hero->registerEventHandler("hited_by_projectile", BIND_EVENT_HANDLER(HeroWidget::_handleEventHitedByProjectile, this));
    _hero->registerEventHandler("play_sound", BIND_EVENT_HANDLER(HeroWidget::_handleEventPlaySound, this));

    return true;
}

void HeroWidget::update(float dt)
{
    setPositionX(_hero->getPositionX());
    setPositionY(_hero->getPositionY());
    
    _dashDistor->setPosition3D(getPosition3D() + cocos2d::Vec3(10.0f, -65.0f, 0.0f));
    
    if (_swordTrail->isVisible())
    {
        float posXCoeff = 0.5f;
        float posYCoeff = 1.0f;
        ItemWeapon *weapon = _hero->getWeapon();
        if (weapon) {
            posYCoeff = weapon->getTrailInfo().posYCoeff;
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
    auto func_end = [&](){_swordTrail->setVisible(false);};
    
    float opacityCoeff = 1.0f;
    ItemWeapon *weapon = _hero->getWeapon();
    if (weapon) {
        opacityCoeff = weapon->getTrailInfo().opacity;
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

void HeroWidget::runForwardDistortion(float time)
{
    auto distorFuncEnd = [&]{_dashDistor->setVisible(false);};
    auto distortionEnd = cocos2d::CallFunc::create(distorFuncEnd);
    auto distortionFadein = cocos2d::FadeIn::create(time * 0.25f);
    auto distortionWait = cocos2d::DelayTime::create(time * 0.75f);
    auto distortionFadeout = cocos2d::FadeOut::create(0.3f);
    auto distortionEffect = cocos2d::Sequence::create(distortionFadein, distortionWait, distortionFadeout, distortionEnd, nullptr);
    _dashDistor->setVisible(true);
    _dashDistor->setOpacity(0);
    _dashDistor->stopAllActions();
    _dashDistor->runAction(distortionEffect);
}

void HeroWidget::runSwirlDistortion(bool flipX)
{
    Effect *swirl = Effect::create("effects/swirl_distortion_01.png");
    swirl->setCameraMask(Effect::TargetDistor);
    swirl->setPosition3D(getPosition3D());
    swirl->setScale(1.7f);
    swirl->setOpacity(0);
    swirl->setAncestor(this);
    swirl->setFlippedX(flipX);
    swirl->scheduleUpdate();
    
    auto swirlEnd = [swirl](){swirl->finish();};
    auto end = cocos2d::CallFunc::create(swirlEnd);
    auto fadein = cocos2d::FadeTo::create(0.1f, 220);
    auto fadewait = cocos2d::DelayTime::create(0.4f);
    auto fadeout = cocos2d::FadeTo::create(0.3f, 0);
    auto fade = cocos2d::Sequence::create(fadein, fadewait, fadeout, nullptr);
    auto scale = cocos2d::ScaleTo::create(1.0f, 2.4f);
    auto rotation = cocos2d::RotateBy::create(0.8f, 180.0f * (flipX ? -1.0f : 1.0f));
    auto rotateAction = cocos2d::Sequence::create(rotation, end, nullptr);
    auto effect = cocos2d::Spawn::create(fade, scale, rotateAction, nullptr);
    
    swirl->runAction(effect);
    _fieldEffects->addChild(swirl);
}

void HeroWidget::_handleEventSwipeRight(const VariablesSet& args)
{
    removeAllAnimations();

    if (_swordSide != _nextSwordSide) {
        _swordSide = _nextSwordSide;
    }

    float time = args.getFloat("duration");

    if (_swordSide == SwordSide::RIGHT) {
        auto bodyAction = AnimBodySwipeRight(time);
        auto swordAction = AnimSwordRightSwipeRight(time);

        _sword->runAction(swordAction);
        _bodyController->runAction(bodyAction);
        _nextSwordSide = SwordSide::RIGHT;

        runSwirlDistortion(true);
    } else if (_swordSide == SwordSide::LEFT) {
        auto swordAction = AnimSwordLeftSwipeRight(time);
        _sword->runAction(swordAction);
        _nextSwordSide = SwordSide::RIGHT;
    }

    runSwordTrailEffect(time);
}

void HeroWidget::_handleEventSwipeLeft(const VariablesSet& args)
{
    removeAllAnimations();

    if (_swordSide != _nextSwordSide) {
        _swordSide = _nextSwordSide;
    }

    float time = args.getFloat("duration");

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

        runSwirlDistortion(false);
    }

    runSwordTrailEffect(time);
}

void HeroWidget::_handleEventJumpBackStart(const VariablesSet& args)
{
    if (args.getBool("showShield", false) && !_shield->isVisible()) {
        _shield->setVisible(true);
        _shield->setOpacity(0);
        _shield->runAction(cocos2d::FadeIn::create(0.1f));
    }

    if (args.getBool("showDistortion", false)) {
        float time = args.getFloat("duration");
        runForwardDistortion(time);
    }
}

void HeroWidget::_handleEventJumpForwardAttack(const VariablesSet& args)
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("whoosh_01.mp3");

    removeAllAnimations();

    if (_swordSide != _nextSwordSide) {
        _swordSide = _nextSwordSide;
    }

    float time = args.getFloat("duration");

    if (_swordSide == SwordSide::RIGHT) {
        auto swordAction = AnimSwordRightSwipeLeft(time);

        _sword->runAction(swordAction);
        _nextSwordSide = SwordSide::LEFT;
    } else if (_swordSide == SwordSide::LEFT) {
        auto swordAction = AnimSwordLeftSwipeRight(time);

        _sword->runAction(swordAction);
        _nextSwordSide = SwordSide::RIGHT;
    }

    runSwordTrailEffect(time);
    runForwardDistortion(time);
}

void HeroWidget::_handleEventHideShield(const VariablesSet& args)
{
    auto func = [this](){_shield->setVisible(false);};
    auto fade = cocos2d::FadeOut::create(0.05f);
    auto call = cocos2d::CallFunc::create(func);
    auto eff = cocos2d::Sequence::create(fade, call, nullptr);

    _shield->runAction(eff);
}

void HeroWidget::_handleEventShieldDamageReceived(const VariablesSet& args)
{
    auto tint0 = cocos2d::TintTo::create(0.1f, cocos2d::Color3B::RED);
    auto tint1 = cocos2d::TintTo::create(0.1f, cocos2d::Color3B::WHITE);
    auto effect = cocos2d::Sequence::create(tint0, tint1, nullptr);

    _shield->runAction(effect);
}

void HeroWidget::_handleEventDamageReceived(const VariablesSet& args)
{
    auto tint0 = cocos2d::TintTo::create(0.1f, cocos2d::Color3B::RED);
    auto tint1 = cocos2d::TintTo::create(0.1f, cocos2d::Color3B::WHITE);
    auto effect = cocos2d::Sequence::create(tint0, tint1, nullptr);
    _body->runAction(effect);

    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("get_damage_01.mp3");
}

void HeroWidget::_handleEventHitedByProjectile(const VariablesSet& args)
{
    cocos2d::Vec3 pos;
    pos.x = args.getFloat("x");
    pos.y = args.getFloat("y");
    pos.z = getPositionZ();

    _fieldEffects->addChild(EffectExplosion::create(pos));
}

void HeroWidget::_handleEventPlaySound(const VariablesSet& args)
{
    std::string soundId = args.getString("soundId");

    if (soundId.length() > 0)
    {
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(soundId.c_str());
    }
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