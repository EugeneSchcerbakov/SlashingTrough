//
//  ScreenСhanger.h
//  SlashingTrough
//
//  Created by Eugene Shcherbakov on 26/08/15.
//
//

#ifndef __SlashingTrough__ScreenСhanger__
#define __SlashingTrough__ScreenСhanger__

#include <cocos2d.h>

#include "FieldLevel.h"
#include "PlayerInfo.h"

class ScreenChanger
{
public:
    typedef const std::string Screen;
    
    static void changeScreen(Screen screen);
    static void beginRunAndSlash(const std::string &levelId);
    static void showStatistics(FieldLevel::WeakPtr level, PlayerInfo::Score score, bool victory);
    
    static Screen START;
    static Screen STORE;
    static Screen MAP;
    static Screen STATISTICS;
    static Screen GAME;
};

#endif /* defined(__SlashingTrough__ScreenСhanger__) */
