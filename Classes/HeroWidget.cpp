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
    
    _swordSide = SwordSide::RIGHT;
    
    scheduleUpdate();
    addChild(_body, 0);
    addChild(_sword, 1);
    
    return true;
}

void HeroWidget::update(float dt)
{
    GameplayObject::Ptr HeroPtr = _hero.lock();
    Hero *Hero = Hero::Cast(HeroPtr);
    
    if (Hero->HasActionToPerform()) {
        HeroAction *action = &Hero->CurrentAction();
        if (action->IsReady() && !_isGameplayActionRunning) {
            action->Start();
            PerformAction(*action);
        }
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

GameplayObject::WeakPtr HeroWidget::GetHero() const
{
    return _hero;
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
                if (heroPtr->Attack(obj, L) && !obj->IsAlive())
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

void HeroWidget::PerformAction(const HeroAction &action)
{
    float deltaX = action.GetDeltaX();
    float deltaY = action.GetDeltaY();
    float duration = action.GetDuration();
    
    std::function<void()> end = [&]() {
        GameplayObject::Ptr ptr = _hero.lock();
        Hero::Cast(ptr)->FinishCurrentAction();
        Hero::Cast(ptr)->SetLogicalPos(getPositionX(), getPositionY());
        _isGameplayActionRunning = false;
    };
    
    cocos2d::Action *anim = nullptr;
    if (action.IsType(HeroAction::Type::SWIPE_RIGHT) && _swordSide == SwordSide::RIGHT) {
        anim = AnimSwordRightSwipeRight(duration);
    } else if (action.IsType(HeroAction::Type::SWIPE_LEFT) && _swordSide == SwordSide::RIGHT) {
        anim = AnimSwordRightSwipeLeft(duration);
    } else if (action.IsType(HeroAction::Type::SWIPE_RIGHT) && _swordSide == SwordSide::LEFT) {
        anim = AnimSwordLeftSwipeRight(duration);
    } else if (action.IsType(HeroAction::Type::SWIPE_LEFT) && _swordSide == SwordSide::LEFT) {
        anim = AnimSwordLeftSwipeLeft(duration);
    } else {
        CC_ASSERT(false);
        return;
    }
    
    float squareSize = GameInfo::Instance().GetFloat("SQUARE_SIZE");
    _attackedRowIndex = (int)floorf(getPositionX() / squareSize);
    
    cocos2d::CallFunc *attack_func = cocos2d::CallFunc::create([&](){Attack();});
    cocos2d::DelayTime *attack_delay = cocos2d::DelayTime::create(duration * 0.3f);
    cocos2d::Sequence *attack = cocos2d::Sequence::create(attack_delay, attack_func, nullptr);
    
    cocos2d::MoveBy *move = cocos2d::MoveBy::create(duration, cocos2d::Vec2(deltaX, deltaY));
    cocos2d::EaseSineInOut *move_ease = cocos2d::EaseSineInOut::create(move);
    cocos2d::Spawn *moution = cocos2d::Spawn::create(move_ease, anim, nullptr);
    cocos2d::CallFunc *func = cocos2d::CallFunc::create(end);
    cocos2d::Sequence *seq = cocos2d::Sequence::create(moution, func, nullptr);
    
    runAction(cocos2d::Spawn::create(seq, attack, nullptr));
    
    _isGameplayActionRunning = true;
}

cocos2d::Action* HeroWidget::AnimSwordRightSwipeRight(float duration)
{
    // sword anim
    cocos2d::RotateBy *sword_rotate0 = cocos2d::RotateBy::create(duration*0.5f, -60.0f);
    cocos2d::RotateBy *sword_rotate1 = cocos2d::RotateBy::create(duration*0.5f, 60.0f);
    cocos2d::Sequence *sword_rotate = cocos2d::Sequence::create(sword_rotate0, sword_rotate1, nullptr);
    _sword->runAction(sword_rotate);
    // body anim
    cocos2d::RotateBy *rotate = cocos2d::RotateBy::create(duration, -360.0f);
    cocos2d::EaseSineInOut *rotate_ease = cocos2d::EaseSineInOut::create(rotate);
    cocos2d::CallFunc *func = cocos2d::CallFunc::create([&](){_swordSide = SwordSide::RIGHT;});
    cocos2d::Sequence *anim = cocos2d::Sequence::create(rotate_ease, func, nullptr);
    return anim;
}

cocos2d::Action* HeroWidget::AnimSwordRightSwipeLeft(float duration)
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

cocos2d::Action* HeroWidget::AnimSwordLeftSwipeRight(float duration)
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

cocos2d::Action* HeroWidget::AnimSwordLeftSwipeLeft(float duration)
{
    // sword anim
    cocos2d::RotateBy *sword_rotate0 = cocos2d::RotateBy::create(duration*0.5f, 60.0f);
    cocos2d::RotateBy *sword_rotate1 = cocos2d::RotateBy::create(duration*0.5f, -60.0f);
    cocos2d::Sequence *sword_rotate = cocos2d::Sequence::create(sword_rotate0, sword_rotate1, nullptr);
    _sword->runAction(sword_rotate);
    // body anim
    cocos2d::RotateBy *rotate = cocos2d::RotateBy::create(duration, 360.0f);
    cocos2d::EaseSineInOut *rotate_ease = cocos2d::EaseSineInOut::create(rotate);
    cocos2d::CallFunc *func = cocos2d::CallFunc::create([&](){_swordSide = SwordSide::LEFT;});
    cocos2d::Sequence *anim = cocos2d::Sequence::create(rotate_ease, func, nullptr);
    return anim;
}

