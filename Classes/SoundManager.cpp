#include "SoundManager.hpp"
#include "SimpleAudioEngine.h"

// iPhone/desktop = wav
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#define EXTENSION ".ogg"
#elif CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID
#define EXTENSION ".wav"
#endif

const char *kSoundEffect_SelectGem   = "sound/gem_select" EXTENSION;
const char *kSoundEffect_DeselectGem = "sound/gem_select" EXTENSION;
const char *kSoundEffect_CastSpell   = "sound/spell_cast" EXTENSION;
const char *kSoundEffect_Fizzle      = "sound/fizzle"     EXTENSION;
const char *kSoundEffect_Click       = "sound/click"      EXTENSION;

SoundManager *SoundManager::instance = nullptr;

SoundManager *SoundManager::get() {
	if (instance == nullptr) {
		instance = new SoundManager;
	}
	return instance;
}

void SoundManager::init() {
	auto instance = CocosDenshion::SimpleAudioEngine::getInstance();
	instance->preloadEffect( kSoundEffect_SelectGem );
	instance->preloadEffect( kSoundEffect_DeselectGem );
	instance->preloadEffect( kSoundEffect_CastSpell );
	instance->preloadEffect( kSoundEffect_Fizzle );
	instance->preloadEffect( kSoundEffect_Click );
}

void SoundManager::playEffect( const char *path ) {
#if CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID
	if (path != nullptr) {
		auto instance = CocosDenshion::SimpleAudioEngine::getInstance();
		instance->playEffect( path );
	}
#endif
}
