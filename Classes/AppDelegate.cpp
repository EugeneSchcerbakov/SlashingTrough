#include "AppDelegate.h"

#include "GameInfo.h"
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
    if(!glview) {
        glview = GLViewImpl::create("Slashing Trough");
        director->setOpenGLView(glview);
    }

    GameInfo::Instance().LoadInfo("gameInfo.xml");
    
    // iPhone5 as reference
    const cocos2d::Size frameSize(640.0f, 1136.0f);
    
    if (Utils::IsPlatformDesctop()) {
        director->setDisplayStats(true);
        glview->setFrameSize(frameSize.width, frameSize.height);
        glview->setFrameZoomFactor(GameInfo::Instance().GetFloat("DESCTOP_FRAME_SCALE", 1.0f));
    }
    
    glview->setDesignResolutionSize(frameSize.width, frameSize.height,
                                    ResolutionPolicy::FIXED_WIDTH);
    
    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);
    
    LuaEngine* engine = LuaEngine::getInstance();
    ScriptEngineManager::getInstance()->setScriptEngine(engine);

    // create a scene. it's an autorelease object
    auto scene = cocos2d::Scene::create();
    
    // run
    director->runWithScene(scene);

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
