//
//  Utils.cpp
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 01/04/15.
//
//

#include "Utils.h"

#include "cocos2d.h"

bool misc::isPlatformDesctop()
{
    cocos2d::Application::Platform platform;
    platform = cocos2d::Application::getInstance()->getTargetPlatform();
    return platform == cocos2d::Application::Platform::OS_MAC ||
           platform == cocos2d::Application::Platform::OS_LINUX ||
           platform == cocos2d::Application::Platform::OS_WINDOWS;
}

int misc::random(int min, int max)
{
    return cocos2d::random(min, max);
}
