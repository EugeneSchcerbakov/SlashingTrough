//
//  Utils.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 01/04/15.
//
//

#ifndef __SlashingTrough__Utils__
#define __SlashingTrough__Utils__

#include "cocos2d.h"

namespace misc
{
    bool isPlatformDesctop();
    
    void luaCallVoidFunction(const std::string &funcName, const std::string &passData);
    void luaSetGlobalInteger(const std::string &varName, int value);
    
    std::string luaGetGlobalString(const std::string &varName);
    
    int random(int min, int max);
    
    cocos2d::Layer* makeLayerFromLua(const std::string &luaFuncName);
    cocos2d::Scene* makeSceneFromLua(const std::string &luaFuncName);
    cocos2d::Node* MakeNodeFromLua(const std::string &luaFuncName);
}

namespace math
{
    template <class T>
    T lerp(T a, T b, float time) {
        return a+time*(b-a);
    }
    
    inline float clampf(float value, float min, float max) {
        if (min > max) {
            std::swap(min, max);
        }
        return value < min ? min : value < max ? value : max;
    }
    
    const float PI = 3.14159265358979323846f;
}

#endif /* defined(__SlashingTrough__Utils__) */
