//
//  HeroWidget.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 06/04/15.
//
//

#include "HeroWidget.h"

#include "GameInfo.h"
#include "Utils.h"
#include "GameplayObjectsWidgets.h"

const HeroWidget::SwordTransform HeroWidget::_swordRightSideTrans(cocos2d::Vec2(60.0f, 0.0f), 160.0f);
const HeroWidget::SwordTransform HeroWidget::_swordLeftSideTrans(cocos2d::Vec2(-60.0f, 0.0f), 160.0f);

HeroWidget* HeroWidget::create(GameplayObject::WeakPtr Hero)
{
    HeroWidget *widget = new HeroWidget(Hero);
    if (widget && widget->init()) {
        widget->autorelease();
    } else {
        delete widget;
        widget = nullptr;
    }
    return widget;
}

HeroWidget::HeroWidget(GameplayObject::WeakPtr hero)
: _hero(hero)
, _sectors(nullptr)
, _isGameplayActionRunning(false)
{
}

HeroWidget::~HeroWidget()
{
}

bool HeroWidget::init()
{
    if (!cocos2d::Node::create()) {
        return false;
    }
    
    _sword = cocos2d::DrawNode::create();
    _body = cocos2d::DrawNode::create();
    _bodyBorder = cocos2d::DrawNode::create();
    
    const cocos2d::Color4F bodyColor = cocos2d::Color4F(0.0f, 0.7f, 1.0f, 0.4f);
    const cocos2d::Color4F borderColor = cocos2d::Color4F(0.0f, 0.0f, 1.0f, 1.0f);
    const float radius = 80.0f;
    
    _bodyBorder->setPosition(cocos2d::Vec2(0.0f, 0.0f));
    _bodyBorder->drawCircle(cocos2d::Vec2(0.0f, 0.0f), radius, 0.0f, 25, false, 1.0f, 0.9f, borderColor);
    
    _body->setPosition(cocos2d::Vec2(0.0f, 0.0f));
    _body->drawSolidCircle(cocos2d::Vec2(0.0f, 0.0f), radius, 0.0f, 25, 1.0f, 0.9f, bodyColor);
    _body->addChild(_bodyBorder, 0);
    
    _sword->drawSegment(cocos2d::Vec2(0.0f, 0.0f), cocos2d::Vec2(0.0f, 250.0f), 9.0f, cocos2d::Color4F::BLACK);
    _sword->setPosition(_swordRightSideTrans.localPos);
    _sword->setRotation(_swordRightSideTrans.angle);
    
    _swordTrail = cocos2d::Sprite::create("sword_trail.png");
    _swordTrail->setPositionY(180.0f);
    _swordTrail->setScale(1.9f);
    _swordTrail->setOpacity(0);
    _swordTrail->setVisible(false);
    
    _swordSide = SwordSide::RIGHT;
    _lastStamina = Hero::Cast(_hero.lock())->GetStaminaPoints();
    
    _bodyControlNode = cocos2d::Node::create();
    _bodyControlNode->addChild(_body, 0);
    _bodyControlNode->addChild(_sword, 1);
    
    scheduleUpdate();
    addChild(_bodyControlNode, 0);
    addChild(_swordTrail, 1);
    
    return true;
}

void HeroWidget::update(float dt)
{
    GameplayObject::Ptr HeroPtr = _hero.lock();
    Hero *hero = Hero::Cast(HeroPtr);
    
    if (hero->HasActionToPerform()) {
        HeroAction *action = &hero->CurrentAction();
        if (action->IsReady() && !_isGameplayActionRunning) {
            action->Start();
            PerformAction(*action);
        }
    }
    
    if (_lastStamina != hero->GetStaminaPoints()) {
        _lastStamina = hero->GetStaminaPoints();
        getEventDispatcher()->dispatchCustomEvent("RefreshInterface");
    }
}

void HeroWidget::RefreshSectorsSequence(PathSectorWidget::SectorsSequence &sectors)
{
    _sectors = &sectors;
}

void HeroWidget::RunEffectReceiveDamage()
{
    cocos2d::ScaleTo *scale0 = cocos2d::ScaleTo::create(0.07f, 0.5f);
    cocos2d::ScaleTo *scale1 = cocos2d::ScaleTo::create(0.2f, 1.0f);
    cocos2d::EaseSineIn *scale_ease0 = cocos2d::EaseSineIn::create(scale0);
    cocos2d::EaseSineOut *scale_ease1 = cocos2d::EaseSineOut::create(scale1);
    cocos2d::Sequence *effect = cocos2d::Sequence::create(scale_ease0, scale_ease1, nullptr);
    runAction(effect);
}

void HeroWidget::RunEffectSwordTrail(float duration)
{
    auto func_start = [&](){_swordTrail->setVisible(true);};
    auto func_end = [&](){_swordTrail->setVisible(false);};
    
    float timeRef = 0.33f;
    float fadeinTime = duration * (0.075f / timeRef);
    float fadeoutTime = duration * (0.13f / timeRef);
    float delayTime = duration * (0.11f / timeRef);
    
    cocos2d::FadeIn *fadein = cocos2d::FadeIn::create(fadeinTime);
    cocos2d::FadeOut *fadeout = cocos2d::FadeOut::create(fadeoutTime);
    cocos2d::DelayTime *dealy = cocos2d::DelayTime::create(delayTime);
    cocos2d::CallFunc *start = cocos2d::CallFunc::create(func_start);
    cocos2d::CallFunc *end = cocos2d::CallFunc::create(func_end);
    cocos2d::Sequence *effect = cocos2d::Sequence::create(start, dealy, fadein,
                                                          fadeout, end, nullptr);
    _swordTrail->runAction(effect);
}

GameplayObject::WeakPtr HeroWidget::GetHero() const
{
    return _hero;
}

HeroWidget::SwordSide HeroWidget::GetSwordSide() const
{
    return _swordSide;
}

void HeroWidget::Attack()
{
    if (!_sectors)
    {
        return;
    }
    
    // pos in the sector space
    cocos2d::Vec2 localPlayer;
    GameplayObject::Ptr heroPtr = _hero.lock();
    
    for (PathSectorWidget *sector : *_sectors)
    {
        PathSector::Ptr path = sector->GetPath();
        PathSector::GameplayObjects &objects = path->GetGameplayObjects();
        localPlayer = sector->convertToNodeSpace(getPosition());
        
        for (GameplayObject::Ptr obj : objects)
        {
            PathSector::Square square = path->GetSquareByObject(obj);
            if (square.x == _attackedRowIndex && localPlayer.y < obj->GetLogicalY())
            {
                float x1 = obj->GetLogicalX();
                float y1 = obj->GetLogicalY();
                float x2 = localPlayer.x;
                float y2 = localPlayer.y;
                float dx = x1 - x2;
                float dy = y1 - y2;
                float L = sqrtf(dx * dx + dy * dy);
                if (heroPtr->Attack(obj, L))
                {
                    auto widget = sector->GetObjectWidget(obj->GetUID());
                    widget->OnDamageReceived(_swordSide);
                    
                    if (!obj->IsAlive())
                    {
                        Hero *hero = Hero::Cast(heroPtr);
                        hero->AddKillPoints(obj->GetRewardKillPoints());
                        hero->AddGoldPoints(obj->GetRewardGoldPoints());
                        hero->AddStaminaPoints(obj->GetRewardStaminaPoints());
                        hero->AddScorePoints(obj->GetRewardScorePoints());
                    
                        getEventDispatcher()->dispatchCustomEvent("RefreshInterface");
                    }
                }
            }
        }
    }
}

void HeroWidget::PerformAction(const HeroAction &action)
{
    float deltaX = action.GetDeltaX();
    float deltaY = action.GetDeltaY();
    float duration = action.GetDuration();
    
    std::function<void()> end = [&]() {
        GameplayObject::Ptr ptr = _hero.lock();
        Hero *hero = Hero::Cast(ptr);
        hero->FinishCurrentAction();
        hero->SetLogicalPos(getPositionX(), getPositionY());
        _isGameplayActionRunning = false;
    };
    
    cocos2d::FiniteTimeAction *moution = nullptr;
    cocos2d::FiniteTimeAction *attack = nullptr;
    
    if (action.IsType(HeroAction::Type::SWIPE_RIGHT) && _swordSide == SwordSide::RIGHT) {
        cocos2d::FiniteTimeAction *anim = AnimSwordRightSwipeRight(duration);
        cocos2d::FiniteTimeAction *move = HorizontalMotion(deltaX, deltaY, duration);
        moution = cocos2d::Spawn::create(move, anim, nullptr);
        attack = HorizontalAttack(duration * 0.3f);
    } else if (action.IsType(HeroAction::Type::SWIPE_LEFT) && _swordSide == SwordSide::RIGHT) {
        cocos2d::FiniteTimeAction *anim = AnimSwordRightSwipeLeft(duration);
        cocos2d::FiniteTimeAction *move = HorizontalMotion(deltaX, deltaY, duration);
        moution = cocos2d::Spawn::create(move, anim, nullptr);
        attack = HorizontalAttack(duration * 0.3f);
    } else if (action.IsType(HeroAction::Type::SWIPE_RIGHT) && _swordSide == SwordSide::LEFT) {
        cocos2d::FiniteTimeAction *anim = AnimSwordLeftSwipeRight(duration);
        cocos2d::FiniteTimeAction *move = HorizontalMotion(deltaX, deltaY, duration);
        moution = cocos2d::Spawn::create(move, anim, nullptr);
        attack = HorizontalAttack(duration * 0.3f);
    } else if (action.IsType(HeroAction::Type::SWIPE_LEFT) && _swordSide == SwordSide::LEFT) {
        cocos2d::FiniteTimeAction *anim = AnimSwordLeftSwipeLeft(duration);
        cocos2d::FiniteTimeAction *move = HorizontalMotion(deltaX, deltaY, duration);
        moution = cocos2d::Spawn::create(move, anim, nullptr);
        attack = HorizontalAttack(duration * 0.3f);
    } else if (action.IsType(HeroAction::Type::JUMP_BACK)) {
        moution = cocos2d::DelayTime::create(duration);
        Hero::Cast(_hero.lock())->JumpBack(duration, deltaY);
    } else {
        CC_ASSERT(false);
        return;
    }
    
    float squareSize = GameInfo::Instance().GetFloat("SQUARE_SIZE");
    _attackedRowIndex = (int)floorf(getPositionX() / squareSize);
    
    cocos2d::CallFunc *actionEndCallback = cocos2d::CallFunc::create(end);
    cocos2d::Sequence *motionWithFinish = cocos2d::Sequence::create(moution, actionEndCallback, nullptr);
    cocos2d::FiniteTimeAction *resultantAction = nullptr;
    
    if (attack) {
        resultantAction = cocos2d::Spawn::create(motionWithFinish, attack, nullptr);
        RunEffectSwordTrail(duration);
    } else {
        resultantAction = motionWithFinish;
    }

    runAction(resultantAction);
    _isGameplayActionRunning = true;
}

cocos2d::FiniteTimeAction* HeroWidget::HorizontalMotion(float deltaX, float deltaY, float time)
{
    cocos2d::MoveBy *move = cocos2d::MoveBy::create(time, cocos2d::Vec2(deltaX, deltaY));
    cocos2d::EaseSineInOut *move_ease = cocos2d::EaseSineInOut::create(move);
    return move_ease;
}

cocos2d::FiniteTimeAction* HeroWidget::HorizontalAttack(float attackTime)
{
    cocos2d::CallFunc *attack_func = cocos2d::CallFunc::create([&](){Attack();});
    cocos2d::DelayTime *attack_delay = cocos2d::DelayTime::create(attackTime);
    cocos2d::Sequence *attack = cocos2d::Sequence::create(attack_delay, attack_func, nullptr);
    return attack;
}

cocos2d::FiniteTimeAction* HeroWidget::AnimSwordRightSwipeRight(float duration)
{
    // sword anim
    cocos2d::RotateBy *sword_rotate0 = cocos2d::RotateBy::create(duration*0.5f, -60.0f);
    cocos2d::RotateBy *sword_rotate1 = cocos2d::RotateBy::create(duration*0.5f, 60.0f);
    cocos2d::Sequence *sword_rotate = cocos2d::Sequence::create(sword_rotate0, sword_rotate1, nullptr);
    _sword->runAction(sword_rotate);
    // body anim
    cocos2d::RotateBy *rotate = cocos2d::RotateBy::create(duration, -360.0f);
    cocos2d::EaseSineInOut *rotate_ease = cocos2d::EaseSineInOut::create(rotate);
    _bodyControlNode->runAction(rotate_ease);
    
    cocos2d::DelayTime *wait = cocos2d::DelayTime::create(duration);
    cocos2d::CallFunc *func = cocos2d::CallFunc::create([&](){_swordSide = SwordSide::RIGHT;});
    cocos2d::Sequence *anim = cocos2d::Sequence::create(wait, func, nullptr);
    return anim;
}

cocos2d::FiniteTimeAction* HeroWidget::AnimSwordRightSwipeLeft(float duration)
{
    // sword anim
    cocos2d::MoveTo *sword_move = cocos2d::MoveTo::create(duration, _swordLeftSideTrans.localPos);
    cocos2d::RotateBy *sword_rotate = cocos2d::RotateBy::create(duration, -320.0f);
    cocos2d::EaseSineInOut *sword_move_ease = cocos2d::EaseSineInOut::create(sword_move);
    cocos2d::EaseSineInOut *sword_rotate_ease = cocos2d::EaseSineInOut::create(sword_rotate);
    cocos2d::Spawn *sword_anim = cocos2d::Spawn::create(sword_move_ease, sword_rotate_ease, nullptr);
    _sword->runAction(sword_anim);
    // no body anim
    cocos2d::CallFunc *func = cocos2d::CallFunc::create([&](){_swordSide = SwordSide::LEFT;});
    cocos2d::DelayTime *wait = cocos2d::DelayTime::create(duration);
    cocos2d::Sequence *anim = cocos2d::Sequence::create(wait, func, nullptr);
    return anim;
}

cocos2d::FiniteTimeAction* HeroWidget::AnimSwordLeftSwipeRight(float duration)
{
    // sword anim
    cocos2d::MoveTo *sword_move = cocos2d::MoveTo::create(duration, _swordRightSideTrans.localPos);
    cocos2d::RotateBy *sword_rotate = cocos2d::RotateBy::create(duration, 320.0f);
    cocos2d::EaseSineInOut *sword_move_ease = cocos2d::EaseSineInOut::create(sword_move);
    cocos2d::EaseSineInOut *sword_rotate_ease = cocos2d::EaseSineInOut::create(sword_rotate);
    cocos2d::Spawn *sword_anim = cocos2d::Spawn::create(sword_move_ease, sword_rotate_ease, nullptr);
    _sword->runAction(sword_anim);
    // no body anim
    cocos2d::CallFunc *func = cocos2d::CallFunc::create([&](){_swordSide = SwordSide::RIGHT;});
    cocos2d::DelayTime *wait = cocos2d::DelayTime::create(duration);
    cocos2d::Sequence *anim = cocos2d::Sequence::create(wait, func, nullptr);
    return anim;
}

cocos2d::FiniteTimeAction* HeroWidget::AnimSwordLeftSwipeLeft(float duration)
{
    // sword anim
    cocos2d::RotateBy *sword_rotate0 = cocos2d::RotateBy::create(duration*0.5f, 60.0f);
    cocos2d::RotateBy *sword_rotate1 = cocos2d::RotateBy::create(duration*0.5f, -60.0f);
    cocos2d::Sequence *sword_rotate = cocos2d::Sequence::create(sword_rotate0, sword_rotate1, nullptr);
    _sword->runAction(sword_rotate);
    // body anim
    cocos2d::RotateBy *rotate = cocos2d::RotateBy::create(duration, 360.0f);
    cocos2d::EaseSineInOut *rotate_ease = cocos2d::EaseSineInOut::create(rotate);
    _bodyControlNode->runAction(rotate_ease);
    
    cocos2d::DelayTime *wait = cocos2d::DelayTime::create(duration);
    cocos2d::CallFunc *func = cocos2d::CallFunc::create([&](){_swordSide = SwordSide::LEFT;});
    cocos2d::Sequence *anim = cocos2d::Sequence::create(wait, func, nullptr);
    return anim;
}

