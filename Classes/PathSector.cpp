//
//  PathSector.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 04/04/15.
//
//

#include "PathSector.h"

PathSector::Ptr PathSector::Create()
{
    return std::make_shared<PathSector>();
}

PathSector::PathSector()
{
    Reset();
}

void PathSector::Generate(int obsticles, int enemies, int squaresByHeight)
{
    _countObsticles = obsticles;
    _countEnemies = enemies;
    _squaresByHeight = squaresByHeight;
}

void PathSector::Reset()
{
    _countObsticles = 0;
    _countEnemies = 0;
    _squaresByHeight = 0;
}

PathSector::ValidationResult PathSector::Validate() const
{
    return ValidationResult::PATH_IS_VALID;
}
