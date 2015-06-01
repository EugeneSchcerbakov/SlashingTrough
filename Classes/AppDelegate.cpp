#include "AppDelegate.h"

#include "ui/lua_cocos2dx_ui_manual.hpp"
#include "cocostudio/lua_cocos2dx_coco_studio_manual.hpp"

#include "GameScene.h"
#include "GameInfo.h"
#include "Store.h"
#include "Utils.h"

USING_NS_CC;

AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() 
{
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
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    auto fileUtils = cocos2d::FileUtils::getInstance();
    auto luaEngine = cocos2d::LuaEngine::getInstance();
	auto frameSize = cocos2d::Size(640.0f, 1136.0f);
    
    ScriptEngineManager::getInstance()->setScriptEngine(luaEngine);

	cocos2d::Application::Platform platform;
	platform = cocos2d::Application::getInstance()->getTargetPlatform();
	if (platform == cocos2d::Application::Platform::OS_WINDOWS) {
		fileUtils->addSearchPath("../../Resources");
		fileUtils->addSearchPath("../../Resources/fonts");
		fileUtils->addSearchPath("../../Resources/textures");
		fileUtils->addSearchPath("../../Resources/scripts");
		luaEngine->addSearchPath("../../Resources/scripts");
	}

	fileUtils->addSearchPath("textures");
    fileUtils->addSearchPath("fonts");
	fileUtils->addSearchPath("scripts");
	luaEngine->addSearchPath("scripts");
    
    if(!glview) {
        glview = GLViewImpl::create("Slashing Trough");
        director->setOpenGLView(glview);
    }
    
	GameInfo::Instance().LoadInfo("gameInfo.xml");
    Store::Instance().LoadStore("storeItems.xml");

    if (Utils::IsPlatformDesctop()) {
        director->setDisplayStats(true);
        glview->setFrameZoomFactor(GameInfo::Instance().GetFloat("DESCTOP_FRAME_SCALE", 1.0f));
        glview->setFrameSize(frameSize.width, frameSize.height);
    }
    
    glview->setDesignResolutionSize(frameSize.width, frameSize.height, ResolutionPolicy::FIXED_WIDTH);
    director->setAnimationInterval(1.0 / 60);

    lua_State *luaState = luaEngine->getLuaStack()->getLuaState();
    register_ui_moudle(luaState);
    register_cocostudio_module(luaState);
    
    luaEngine->executeScriptFile("gui.lua");
    
    float squareSize = director->getVisibleSize().width / 3.0f;
    GameInfo::Instance().SetFloat("SQUARE_SIZE", squareSize);
    GameInfo::Instance().SetFloat("PATH_LEFT_BORDER", squareSize);
    GameInfo::Instance().SetFloat("PATH_RIGHT_BORDER", squareSize * 2.0f);
    GameInfo::Instance().SetInt("CHARACTER_SCORE", 0);
    
    cocos2d::Scene *scene = Utils::MakeSceneFromLua("CreateStartScene");
    
    auto OnStartPressed = [&](cocos2d::EventCustom *)
    {
        GameScene *gs = GameScene::create();
        cocos2d::Director *director;
        director = cocos2d::Director::getInstance();
        director->replaceScene(gs);
    };
    
    if (scene) {
        cocos2d::EventDispatcher *dispatcher;
        dispatcher = scene->getEventDispatcher();
        dispatcher->addCustomEventListener("StartButtonPressed", OnStartPressed);
        director->runWithScene(scene);
    } else {
        return false;
    }

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
