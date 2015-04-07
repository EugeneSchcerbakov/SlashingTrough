//
//  Character.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 06/04/15.
//
//

#include "Character.h"

#include "GameInfo.h"

Character::Ptr Character::Create()
{
    return std::make_shared<Character>();
}

Character::Character()
{
    Init();
}

void Character::AddHealth(float health)
{
    _healthPoints += health;
}

float Character::GetRunningSpeed() const
{
    return _runningSpeed;
}

bool Character::IsAlive() const
{
    return _healthPoints > 0.0f;
}

void Character::Init()
{
    GameInfo &gameinfo = GameInfo::Instance();
    
    _runningSpeed = gameinfo.GetFloat("CHARACTER_RUNNING_SPEED");
    _attackDamage = gameinfo.GetFloat("CHARACTER_ATTACK_DAMAGE");
    _healthPoints = gameinfo.GetFloat("CHARACTER_HEALTH_POINTS");
}

