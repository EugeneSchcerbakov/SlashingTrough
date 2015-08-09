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

bool misc::isPlatformDesctop()
{
    cocos2d::Application::Platform platform;
    platform = cocos2d::Application::getInstance()->getTargetPlatform();
    return platform == cocos2d::Application::Platform::OS_MAC ||
           platform == cocos2d::Application::Platform::OS_LINUX ||
           platform == cocos2d::Application::Platform::OS_WINDOWS;
}

void misc::luaCallVoidFunction(const std::string &funcName, const std::string &passData)
{
    cocos2d::LuaEngine *luaEngine = cocos2d::LuaEngine::getInstance();
    lua_State *L = luaEngine->getLuaStack()->getLuaState();
    
    lua_getglobal(L, funcName.c_str());
    lua_pushstring(L, passData.c_str());
    lua_call(L, 1, 0);
}

void misc::luaSetGlobalInteger(const std::string &varName, int value)
{
    cocos2d::LuaEngine *luaEngine = cocos2d::LuaEngine::getInstance();
    std::string command = cocos2d::StringUtils::format("%s = %d", varName.c_str(), value);
    luaEngine->executeString(command.c_str());
}

std::string misc::luaGetGlobalString(const std::string &varName)
{
    cocos2d::LuaEngine *luaEngine = cocos2d::LuaEngine::getInstance();
    lua_State *L = luaEngine->getLuaStack()->getLuaState();
    
    lua_getglobal(L, varName.c_str());
    if (!lua_isstring(L, -1)) {
        return "";
    }
    return lua_tostring(L, -1);
}

int misc::random(int min, int max)
{
    return cocos2d::random(min, max);
}

cocos2d::Layer* misc::makeLayerFromLua(const std::string &luaFuncName)
{
    cocos2d::LuaEngine *luaEngine = cocos2d::LuaEngine::getInstance();
    lua_State *luaState = luaEngine->getLuaStack()->getLuaState();
    return MakeFromLua<cocos2d::Layer>(luaState, luaFuncName, "cc.Layer");
}

cocos2d::Scene* misc::makeSceneFromLua(const std::string &luaFuncName)
{
    cocos2d::LuaEngine *luaEngine = cocos2d::LuaEngine::getInstance();
    lua_State *luaState = luaEngine->getLuaStack()->getLuaState();
    return MakeFromLua<cocos2d::Scene>(luaState, luaFuncName, "cc.Scene");
}

cocos2d::Node* misc::MakeNodeFromLua(const std::string &luaFuncName)
{
    cocos2d::LuaEngine *luaEngine = cocos2d::LuaEngine::getInstance();
    lua_State *luaState = luaEngine->getLuaStack()->getLuaState();
    return MakeFromLua<cocos2d::Node>(luaState, luaFuncName, "cc.Node");
}
