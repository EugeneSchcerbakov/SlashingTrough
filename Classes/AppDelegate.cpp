#include "AppDelegate.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "PluginChartboost/PluginChartboost.h"
#endif

#include "mp_cocos.h"
#include "platform_cocos.h"

#include "audio/include/SimpleAudioEngine.h"

#include "ScreenChanger.h"
#include "GameInfo.h"
#include "PlayerInfo.h"
#include "FieldSector.h"
#include "LevelsCache.h"
#include "Store.h"
#include "DailyMissions.h"
#include "Utils.h"
#include "Log.h"
#include "Cheats.h"

USING_NS_CC;

AppDelegate::AppDelegate()
: _device(nullptr)
{

}

AppDelegate::~AppDelegate() 
{
    if (_device)
    {
        _device->Destroy();
        delete _device;
        _device = nullptr;
    }
    
    MP_Manager& mp = MP_Manager::GetInstance();
    mp.Destroy();
}

//if you want a different context,just modify the value of glContextAttrs
//it will takes effect on all platforms
void AppDelegate::initGLContextAttrs()
{
    //set OpenGL context attributions,now can only set six attributions:
    //red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

    GLView::setGLContextAttrs(glContextAttrs);
}

bool AppDelegate::applicationDidFinishLaunching() {
    WRITE_INIT("Slashing Through started.");
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    sdkbox::PluginChartboost::init();
#endif
    
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    auto fileUtils = cocos2d::FileUtils::getInstance();
	auto frameSize = cocos2d::Size(640.0f, 1136.0f);

	cocos2d::Application::Platform targetPlatform;
	targetPlatform = cocos2d::Application::getInstance()->getTargetPlatform();
	if (targetPlatform == cocos2d::Application::Platform::OS_WINDOWS) {
		fileUtils->addSearchPath("../../Resources");
		fileUtils->addSearchPath("../../Resources/fonts");
		fileUtils->addSearchPath("../../Resources/textures");
        fileUtils->addSearchPath("../../Resources/audio");
	}

	fileUtils->addSearchPath("textures");
    fileUtils->addSearchPath("fonts");
    fileUtils->addSearchPath("audio");
    
    if(!glview) {
        glview = GLViewImpl::create("Slashing Trough");
        director->setOpenGLView(glview);
    }
    
    cocos2d::Size clientSize = director->getWinSizeInPixels();
    _device = new MP_Device_Cocos(clientSize.width, clientSize.height);
    _device->Create();
    
    MP_Manager& MP=MP_Manager::GetInstance();
    
    MP_Platform* platform=new MP_Platform_COCOS;
    MAGIC_AXIS_ENUM axis=MAGIC_pXpYnZ;
    
#ifdef SHADER_WRAP
    bool filters[MAGIC_RENDER_STATE__MAX];
    for (int i=0;i<MAGIC_RENDER_STATE__MAX;i++)
        filters[i]=false;
    filters[MAGIC_RENDER_STATE_BLENDING]=true;
    filters[MAGIC_RENDER_STATE_TEXTURE]=true;
    filters[MAGIC_RENDER_STATE_ADDRESS_U]=true;
    filters[MAGIC_RENDER_STATE_ADDRESS_V]=true;
    filters[MAGIC_RENDER_STATE_ZENABLE]=true;
    filters[MAGIC_RENDER_STATE_ZWRITE]=true;
#ifndef SHADER_ALPHATEST_WRAP
    filters[MAGIC_RENDER_STATE_ALPHATEST_INIT]=true;
    filters[MAGIC_RENDER_STATE_ALPHATEST]=true;
#endif
#else
    bool* filters=NULL;
#endif
    
    MP.Initialization(filters, true, axis, platform, MAGIC_INTERPOLATION_ENABLE, MAGIC_CHANGE_EMITTER_DEFAULT, 1024, 1024, 1, 1.f, 0.1f, true);
    MP.LoadAllEmitters();
    MP.RefreshAtlas();
    MP.CloseFiles();
    MP.Stop();
    
	GameInfo::getInstance().loadInfo("gameInfo.xml");
    PresetsLibrary::getInstance().load("presets.xml");
    LevelsCache::getInstance().load("levels.xml");
    Store::getInstance().loadStore("storeItems.xml");
    DailyMissions::getInstance().loadMissions("dailyMissions.xml");
    PlayerInfo::getInstance().load("st_save.xml");
        
    DailyMissions::getInstance().checkMastering();
    DailyMissions::getInstance().checkMissionsSwitchTime();

    Cheats::initBasicCheats();
    
    if (misc::isPlatformDesctop()) {
        director->setDisplayStats(true);
        glview->setFrameZoomFactor(GameInfo::getInstance().getConstFloat("DESCTOP_FRAME_SCALE", 1.0f));
        glview->setFrameSize(frameSize.width, frameSize.height);
    }
    
    glview->setDesignResolutionSize(frameSize.width, frameSize.height, ResolutionPolicy::FIXED_WIDTH);
    director->setAnimationInterval(1.0f / 60.0f);
    
    ScreenChanger::changeScreen(ScreenChanger::START);

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}
