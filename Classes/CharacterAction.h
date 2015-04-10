//
//  CharacterAction.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 09/04/15.
//
//

#ifndef __SlashingTrough__CharacterAction__
#define __SlashingTrough__CharacterAction__

class CharacterAction
{
public:
    enum Type
    {
        NONE,
        SWIPE_RIGHT,
        SWIPE_LEFT
    };
    
    friend class Character;
    
public:
    CharacterAction(Type type, float duration, float deltaX, float deltaY);
    CharacterAction();
    
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

#endif /* defined(__SlashingTrough__CharacterAction__) */
