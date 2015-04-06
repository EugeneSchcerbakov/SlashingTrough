//
//  PathSector.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 04/04/15.
//
//

#ifndef __SlashingTrough__PathSector__
#define __SlashingTrough__PathSector__

#include <memory>

class PathSector
{
public:
    enum class ValidationResult
    {
        PATH_IS_VALID,
        PASSAGE_NOT_EXIST,
        NOT_ENOUGH_SPACE
    };
    
    typedef std::shared_ptr<PathSector> Ptr;
    typedef std::weak_ptr<PathSector> WeakPtr;
    
public:
    static Ptr Create();
    
    PathSector();
    
    void Generate(int obsticles, int enemies, int squaresByHeight);
    void Reset();
    
    inline int GetObsticlesCount() const {return _countObsticles;}
    inline int GetEmeniesCount() const {return _countEnemies;}
    inline int GetSquaresByHeight() const {return _squaresByHeight;}
    
    ValidationResult Validate() const;
    
private:
    int _countObsticles;
    int _countEnemies;
    int _squaresByHeight;
};

#endif /* defined(__SlashingTrough__PathSector__) */
