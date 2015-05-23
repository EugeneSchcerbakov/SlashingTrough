//
//  HeroAction.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 09/04/15.
//
//

#ifndef __SlashingTrough__HeroAction__
#define __SlashingTrough__HeroAction__

class HeroAction
{
public:
    enum Type
    {
        NONE,
        SWIPE_RIGHT,
        SWIPE_LEFT,
        JUMP_BACK
    };
    
    friend class Hero;
    
public:
    HeroAction(Type type, float duration, float deltaX, float deltaY);
    HeroAction();
    
    bool IsType(Type type) const;
    float GetFinishX() const;
    float GetFinishY() const;
    float GetDeltaX() const;
    float GetDeltaY() const;
    float GetDuration() const;
    bool IsReady() const;
    
    void Start();
    void Finish();
    
private:
    void SetFinishPosX(float x);
    void SetFinishPosY(float y);
    
    Type _type;
    float _duration;
    float _deltaX;
    float _deltaY;
    float _finishPosX;
    float _finishPosY;
    bool _isProcessing;
    bool _isFinished;
};

#endif /* defined(__SlashingTrough__HeroAction__) */
