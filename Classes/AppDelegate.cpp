#include "AppDelegate.h"
#include "GameScene.hpp"
#include "Strings.hpp"
#include "GameController.hpp"
#include "Level.hpp"
#include "Monster.hpp"

#define SIZE_IPHONE_3  Size(320, 480)
#define SIZE_IPHONE_4  Size(640, 960)
#define SIZE_IPHONE_5  Size(640, 1136)
#define SIZE_IPHONE_6  Size(750, 1334)
#define SIZE_IPHONE_6P Size(1080, 1920)
#define SIZE_IPAD_RET  Size(1536, 2048)
#define SIZE_IPAD      Size(768, 1024)
static Size screenSize = SIZE_IPHONE_3;

static Size designResolutionSize = Size(320, 568);

static Size smallResolutionSize  = Size(320, 568);
static Size mediumResolutionSize = Size(640, 1136);
static Size largeResolutionSize  = Size(1536, 2048);

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

// If you want to use packages manager to install more packages, 
// don't modify or remove this function
static int register_all_packages()
{
    return 0; //flag for packages manager
}

bool AppDelegate::applicationDidFinishLaunching() {	
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
        glview = GLViewImpl::createWithRect("GemGame", Rect(0, 0, screenSize.width, screenSize.height));
#else
        glview = GLViewImpl::create("GemGame");
#endif
        director->setOpenGLView(glview);
    }

    // turn on display FPS
    director->setDisplayStats(true);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);

    // Set the design resolution
    glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, ResolutionPolicy::FIXED_WIDTH);
    Size frameSize = glview->getFrameSize();
    
    // For now, always scale as per medium resolution.
		// ie will need 50% and 150% assets
	director->setContentScaleFactor(MIN(mediumResolutionSize.height/designResolutionSize.height, mediumResolutionSize.width/designResolutionSize.width));
    /*
    // if the frame's height is larger than the height of medium size.
    if (frameSize.height > mediumResolutionSize.height)
    {        
        director->setContentScaleFactor(MIN(largeResolutionSize.height/designResolutionSize.height, largeResolutionSize.width/designResolutionSize.width));
    }
    // if the frame's height is larger than the height of small size.
    else if (frameSize.height > smallResolutionSize.height)
    {        
        director->setContentScaleFactor(MIN(mediumResolutionSize.height/designResolutionSize.height, mediumResolutionSize.width/designResolutionSize.width));
    }
    // if the frame's height is smaller than the height of medium size.
    else
    {        
        director->setContentScaleFactor(MIN(smallResolutionSize.height/designResolutionSize.height, smallResolutionSize.width/designResolutionSize.width));
    }
    */
    
    register_all_packages();
	
	// Initalise objects like strings, sprites etc.
	// load the Sprite Sheet
	auto spritecache = SpriteFrameCache::getInstance();
	spritecache->addSpriteFramesWithFile("spritesheet.plist");
	
	// Init JSON things
	SpellManager::get()->init();
	MonsterManager::get()->init();
	LevelManager::get()->init();
	Strings::get()->init();
	
	
	// Random
	srand (time(NULL));
	
	
	auto scene = Scene::create();
	GameController::init(scene);
	GameController::get()->setState(kStateMainMenu);
	//GameController::get()->setState(kStateGame);
	
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
