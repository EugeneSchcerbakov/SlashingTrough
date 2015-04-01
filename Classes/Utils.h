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
    
    cocos2d::Layer* MakeLayerFromLua(const std::string &luaFuncName);
    cocos2d::Scene* MakeSceneFromLua(const std::string &luaFuncName);
    cocos2d::Node* MakeNodeFromLua(const std::string &luaFuncName);
}

#endif /* defined(__SlashingTrough__Utils__) */
