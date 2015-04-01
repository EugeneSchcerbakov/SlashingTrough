//
//  Utils.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 01/04/15.
//
//

#include "Utils.h"

#include "CCLuaEngine.h"
#include "LuaBasicConversions.h"
#include "base/CCScriptSupport.h"

template <typename T> T* MakeFromLua(lua_State *state, const std::string &func, const std::string &type)
{
    T *object = nullptr;
    
    //get function that returns a Node
    lua_getglobal(state, func.c_str());
    //call function
    lua_call(state, 0, 1);
    //retrieve return value
    luaval_to_object<T>(state, -1, type.c_str(), &object);
    //remove return value from statck
    lua_pop(state, 1);
    
    return object;
}

cocos2d::Layer* Utils::MakeLayerFromLua(const std::string &luaFuncName)
{
    cocos2d::LuaEngine *luaEngine = cocos2d::LuaEngine::getInstance();
    lua_State *luaState = luaEngine->getLuaStack()->getLuaState();
    return MakeFromLua<cocos2d::Layer>(luaState, luaFuncName, "cc.Layer");
}

cocos2d::Scene* Utils::MakeSceneFromLua(const std::string &luaFuncName)
{
    cocos2d::LuaEngine *luaEngine = cocos2d::LuaEngine::getInstance();
    lua_State *luaState = luaEngine->getLuaStack()->getLuaState();
    return MakeFromLua<cocos2d::Scene>(luaState, luaFuncName, "cc.Scene");
}

cocos2d::Node* Utils::MakeNodeFromLua(const std::string &luaFuncName)
{
    cocos2d::LuaEngine *luaEngine = cocos2d::LuaEngine::getInstance();
    lua_State *luaState = luaEngine->getLuaStack()->getLuaState();
    return MakeFromLua<cocos2d::Node>(luaState, luaFuncName, "cc.Node");
}
