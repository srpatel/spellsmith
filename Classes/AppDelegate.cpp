#include "AppDelegate.h"
#include "GameScene.hpp"
#include "Strings.hpp"
#include "GameController.hpp"
#include "Level.hpp"
#include "Monster.hpp"
#include "ImageManager.hpp"
#include "SoundManager.hpp"
#include "SaveData.hpp"

#include "spine.h"

#define SIZE_IPHONE_3  Size(320, 480)
#define SIZE_IPHONE_4  Size(640, 960)
#define SIZE_IPHONE_5  Size(640, 1136)
#define SIZE_IPHONE_6  Size(750, 1334)
#define SIZE_IPHONE_6P Size(1080, 1920)
#define SIZE_IPAD_RET  Size(1536, 2048)
#define SIZE_IPAD      Size(768, 1024)
static Size screenSize = SIZE_IPHONE_5/2;
//static Size screenSize = SIZE_IPAD_RET/4;

static Size designResolutionSize = Size(320, 568);

static Size smallResolutionSize  = Size(320, 568);
static Size mediumResolutionSize = Size(640, 1136);
static Size largeResolutionSize  = Size(1536, 2048);

AppDelegate::AppDelegate() {
	std::srand(time(0));
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
#if DESKTOP && 0
    director->setDisplayStats(true);
#else
	director->setDisplayStats(false);
#endif

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);

    // Set the design resolution
    glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, ResolutionPolicy::FIXED_WIDTH);
    Size frameSize = glview->getFrameSize();
	printf("Frame size: %g x %g\n", frameSize.width, frameSize.height);
#if USE_SPRITE_SHEET
	// load the Sprite Sheet
	auto spritecache = SpriteFrameCache::getInstance();
	
    // if the frame's height is larger than the height of medium size.
	// TODO : Load all spritesheets ending with the correct prefix.
    /*if (frameSize.height > mediumResolutionSize.height)
    {
        director->setContentScaleFactor(3);
		spritecache->addSpriteFramesWithFile("spritesheet0-ipadhd.plist");
		spritecache->addSpriteFramesWithFile("spritesheet1-ipadhd.plist");
    }
    // if the frame's height is larger than the height of small size.
    else*/
	spAtlas* atlas;
	if (frameSize.height > smallResolutionSize.height)
    {        
        director->setContentScaleFactor(2);
		atlas = spAtlas_createFromFile("atli/spritesheet-hd.atlas", 0);
    }
    // if the frame's height is smaller than the height of medium size.
    else
    {        
        director->setContentScaleFactor(1);
		atlas = spAtlas_createFromFile("atli/spritesheet-base.atlas", 0);
	}
	
	printf("Content scale factor: %g\n", director->getContentScaleFactor());
	
	
	spAtlasRegion* region = atlas->regions;
	while (region) {
		//printf("Page name: %s\n", region->page->name);
		//printf("Regi name: %s\n", region->name);
		SpriteFrame* sf = SpriteFrame::create(
			std::string("atli/") + region->page->name,
			Rect(region->x, region->y, region->width, region->height),
			(bool)region->rotate,
			Point(region->offsetX, region->offsetY),
			Size(region->originalWidth, region->originalHeight));
		spritecache->addSpriteFrame(sf, std::string(region->name) + ".png");
		region = region->next;
	}
	
	std::vector<std::string> searchPaths;
	// Should change this depending on the scale...
	searchPaths.push_back("large");
	FileUtils::getInstance()->setSearchPaths(searchPaths);
#else
	director->setContentScaleFactor(2);
	std::vector<std::string> searchPaths;
	searchPaths.push_back("/Users/sunil/projects/Gems/Gems/DevResources/images-prepack/");
	FileUtils::getInstance()->setSearchPaths(searchPaths);
#endif
    
    register_all_packages();
	
	// Init JSON things
	ImageManager::get()->init(atlas);
	SpellManager::get()->init();
	MonsterManager::get()->init();
	Strings::get()->init();
	SoundManager::get()->init();
	SaveData::init();
		
	// Warn about missing translations
	for (Spell *spell : SpellManager::get()->spells) {
		spell->getName();
	}
	
	// Random
	srand (time(NULL));
	
	
	auto scene = Scene::create();
	GameController::init(scene);
	//GameController::get()->setState(kStateMainMenu);
	GameController::get()->setState(kStateMap);

    // run
    director->runWithScene(scene);

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    // if you use SimpleAudioEngine, it must be paused
    CocosDenshion::SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must be resumed here
    CocosDenshion::SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}
