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

namespace Utils
{
    bool IsPlatformDesctop();
    
    void LuaCallVoidFunction(const std::string &funcName, const std::string &passData);
    void LuaSetGlobalInteger(const std::string &varName, int value);
    
    cocos2d::Layer* MakeLayerFromLua(const std::string &luaFuncName);
    cocos2d::Scene* MakeSceneFromLua(const std::string &luaFuncName);
    cocos2d::Node* MakeNodeFromLua(const std::string &luaFuncName);
}

namespace math
{
    template <class T>
    T lerp(T a, T b, float time) {
        return a+time*(b-a);
    }
}

#endif /* defined(__SlashingTrough__Utils__) */
