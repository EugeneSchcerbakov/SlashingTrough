//
//  Utils.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 01/04/15.
//
//

#ifndef __SlashingTrough__Utils__
#define __SlashingTrough__Utils__

#include <math.h>
#include <utility>
#include <algorithm>

namespace misc
{
    bool isPlatformDesctop();
    
    int random(int min, int max);
}

namespace math
{
    template <class T>
    T lerp(T a, T b, float time) {
        return a+time*(b-a);
    }
    
    template <class T>
    inline T clamp(T value, T min, T max) {
        if (min > max) {
            std::swap(min, max);
        }
        return value < min ? min : value < max ? value : max;
    }
    
    bool samef(float a, float b);
    
    const float PI = 3.14159265358979323846f;
}

#endif /* defined(__SlashingTrough__Utils__) */
