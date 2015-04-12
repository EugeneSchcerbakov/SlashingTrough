//
//  CharacterWidget.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 06/04/15.
//
//

#include "CharacterWidget.h"

#include "GameInfo.h"

const CharacterWidget::SwordTransform CharacterWidget::_swordRightSideTrans(cocos2d::Vec2(60.0f, 0.0f), 160.0f);
const CharacterWidget::SwordTransform CharacterWidget::_swordLeftSideTrans(cocos2d::Vec2(-60.0f, 0.0f), 160.0f);

CharacterWidget* CharacterWidget::create(Character::WeakPtr character)
{
    CharacterWidget *widget = new CharacterWidget(character);
    if (widget && widget->init()) {
        widget->autorelease();
    } else {
        delete widget;
        widget = nullptr;
    }
    return widget;
}

CharacterWidget::CharacterWidget(Character::WeakPtr character)
: _character(character)
{
}

CharacterWidget::~CharacterWidget()
{
}

bool CharacterWidget::init()
{
    if (!cocos2d::Node::create()) {
        return false;
    }
    
    _sword = cocos2d::DrawNode::create();
    _body = cocos2d::DrawNode::create();
    _head = cocos2d::DrawNode::create();
    _bodyBorder = cocos2d::DrawNode::create();
    
    const cocos2d::Color4F bodyColor = cocos2d::Color4F(0.0f, 0.7f, 1.0f, 0.4f);
    const cocos2d::Color4F borderColor = cocos2d::Color4F(0.0f, 0.0f, 1.0f, 1.0f);
    const float radius = 80.0f;
    
    _bodyBorder->setPosition(cocos2d::Vec2(0.0f, 0.0f));
    _bodyBorder->drawCircle(cocos2d::Vec2(0.0f, 0.0f), radius, 0.0f, 25, false, 1.0f, 0.9f, borderColor);
    
    _head->setPosition(cocos2d::Vec2(0.0f, radius * 0.9f));
    _head->drawSolidCircle(cocos2d::Vec2(0.0f, 0.0f), 15.0f, 0.0f, 10, 1.0f, 1.0f, cocos2d::Color4F::MAGENTA);
    
    _body->setPosition(cocos2d::Vec2(0.0f, 0.0f));
    _body->drawSolidCircle(cocos2d::Vec2(0.0f, 0.0f), radius, 0.0f, 25, 1.0f, 0.9f, bodyColor);
    _body->addChild(_bodyBorder, 0);
    _body->addChild(_head, 1);
    
    _sword->drawSegment(cocos2d::Vec2(0.0f, 0.0f), cocos2d::Vec2(0.0f, 250.0f), 9.0f, cocos2d::Color4F::BLACK);
    _sword->setPosition(_swordRightSideTrans.localPos);
    _sword->setRotation(_swordRightSideTrans.angle);
    
    _swordSide = SwordSide::RIGHT;
    
    scheduleUpdate();
    addChild(_body, 0);
    addChild(_sword, 1);
    
    return true;
}

void CharacterWidget::update(float dt)
{
    Character::Ptr characterPtr = _character.lock();
    
    if (characterPtr->HasActionToPerform()) {
        CharacterAction *action = &characterPtr->CurrentAction();
        if (action->IsReady() && getNumberOfRunningActions() == 0) {
            action->Start();
            PerformAction(*action);
        }
    }
}

void CharacterWidget::PerformAction(const CharacterAction &action)
{
    float deltaX = action.GetDeltaX();
    float deltaY = action.GetDeltaY();
    float duration = action.GetDuration();
    
    std::function<void()> end = [&]() {
        Character::Ptr ptr = _character.lock();
        ptr->FinishCurrentAction();
        ptr->SetLogicalPos(getPositionX(), getPositionY());
    };
    
    cocos2d::Action *anim = nullptr;
    if (action.IsType(CharacterAction::Type::SWIPE_RIGHT) && _swordSide == SwordSide::RIGHT) {
        anim = AnimSwordRightSwipeRight(duration);
    } else if (action.IsType(CharacterAction::Type::SWIPE_LEFT) && _swordSide == SwordSide::RIGHT) {
        anim = AnimSwordRightSwipeLeft(duration);
    } else if (action.IsType(CharacterAction::Type::SWIPE_RIGHT) && _swordSide == SwordSide::LEFT) {
        anim = AnimSwordLeftSwipeRight(duration);
    } else if (action.IsType(CharacterAction::Type::SWIPE_LEFT) && _swordSide == SwordSide::LEFT) {
        anim = AnimSwordLeftSwipeLeft(duration);
    } else {
        CC_ASSERT(false);
        return;
    }
    
    cocos2d::MoveBy *move = cocos2d::MoveBy::create(duration, cocos2d::Vec2(deltaX, deltaY));
    cocos2d::EaseSineInOut *move_ease = cocos2d::EaseSineInOut::create(move);
    cocos2d::Spawn *moution = cocos2d::Spawn::create(move_ease, anim, nullptr);
    cocos2d::CallFunc *func = cocos2d::CallFunc::create(end);
    cocos2d::Sequence *seq = cocos2d::Sequence::create(moution, func, nullptr);
    
    runAction(seq);
}

cocos2d::Action* CharacterWidget::AnimSwordRightSwipeRight(float duration)
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

cocos2d::Action* CharacterWidget::AnimSwordRightSwipeLeft(float duration)
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

cocos2d::Action* CharacterWidget::AnimSwordLeftSwipeRight(float duration)
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

cocos2d::Action* CharacterWidget::AnimSwordLeftSwipeLeft(float duration)
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

