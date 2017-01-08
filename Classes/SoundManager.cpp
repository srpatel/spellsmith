#include "SoundManager.hpp"
#include "SimpleAudioEngine.h"

// iPhone/desktop = wav
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#define EXTENSION ".ogg"
#elif CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID
#define EXTENSION ".wav"
#endif

const char *kSoundEffectSelectGem = "sound/gem_select" EXTENSION;
const char *kSoundEffectDeselectGem = nullptr;

SoundManager *SoundManager::instance = nullptr;

SoundManager *SoundManager::get() {
	if (instance == nullptr) {
		instance = new SoundManager;
	}
	return instance;
}

void SoundManager::init() {
	//auto instance = CocosDenshion::SimpleAudioEngine::getInstance();
	//instance->preloadEffect( ... );
}

void SoundManager::playEffect( const char *path ) {
#if CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID
	if (path != nullptr) {
		auto instance = CocosDenshion::SimpleAudioEngine::getInstance();
		instance->playEffect( path );
	}
#endif
}
