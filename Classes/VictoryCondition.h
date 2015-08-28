//
//  VictoryCondition.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 27/08/15.
//
//

#ifndef __SlashingTrough__VictoryCondition__
#define __SlashingTrough__VictoryCondition__

#include <memory>

class Hero;
class FieldLevel;

class VictoryCondition
{
public:
    enum Result
    {
        NONE,
        FAILED,
        VIRTORY
    };
    
public:
    typedef std::shared_ptr<VictoryCondition> Ptr;
    typedef std::weak_ptr<VictoryCondition> WeakPtr;
    
public:
    VictoryCondition(FieldLevel *level);
    virtual ~VictoryCondition();
    
    virtual void init(Hero *hero);
    virtual void update(float dt);
    
    bool isResult(Result result) const;
    
protected:
    Result _result;
    Hero *_hero;
    FieldLevel *_level;
};

class ClassicCondition : public VictoryCondition
{
public:
    static VictoryCondition::Ptr create(FieldLevel *level);
    
public:
    ClassicCondition(FieldLevel *level);

    void update(float dt) override;
};

#endif /* defined(__SlashingTrough__VictoryCondition__) */
