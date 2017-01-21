#include "SoundManager.hpp"

// iPhone/desktop = wav
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#define EXTENSION ".ogg"
#elif CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID
#define EXTENSION ".wav"
#endif

const char *kSoundEffect_SelectGem   = "sound/gem_select" EXTENSION;
const char *kSoundEffect_DeselectGem = "sound/gem_deselect" EXTENSION;
const char *kSoundEffect_Cast        = "sound/cast" EXTENSION;
const char *kSoundEffect_Fizzle      = "sound/fizzle" EXTENSION;
const char *kSoundEffect_Hum         = "sound/hum" EXTENSION;

const char *kSoundEffect_LevelLose   = "sound/game_over_lose" EXTENSION;
const char *kSoundEffect_LevelWin    = "sound/level_completed" EXTENSION;

const char *kSoundEffect_PHAir       = "sound/projectile_hit_air" EXTENSION;
const char *kSoundEffect_PHWater     = "sound/projectile_hit_water" EXTENSION;
const char *kSoundEffect_PHEarth     = "sound/projectile_hit_earth" EXTENSION;
const char *kSoundEffect_PHFire      = "sound/projectile_hit_fire" EXTENSION;

const char *kSoundEffect_Thwack      = "sound/weapon_hit" EXTENSION;

const char *kSoundEffect_UISelect      = "sound/ui_select" EXTENSION;
const char *kSoundEffect_UISelectMinor = "sound/ui_select_minor" EXTENSION;
const char *kSoundEffect_UIBack        = "sound/ui_back" EXTENSION;

SoundManager *SoundManager::instance = nullptr;

SoundManager *SoundManager::get() {
	if (instance == nullptr) {
		instance = new SoundManager;
	}
	return instance;
}

void SoundManager::init() {
	auto instance = CocosDenshion::SimpleAudioEngine::getInstance();
	
	instance->preloadEffect( kSoundEffect_UISelect );
	instance->preloadEffect( kSoundEffect_UISelectMinor );
	instance->preloadEffect( kSoundEffect_UIBack );
	
	game.push_back(kSoundEffect_LevelWin);
	game.push_back(kSoundEffect_LevelLose);
	game.push_back(kSoundEffect_PHAir);
	game.push_back(kSoundEffect_PHWater);
	game.push_back(kSoundEffect_PHEarth);
	game.push_back(kSoundEffect_PHFire);
	game.push_back(kSoundEffect_SelectGem);
	game.push_back(kSoundEffect_DeselectGem);
	game.push_back(kSoundEffect_Hum);
	game.push_back(kSoundEffect_Fizzle);
	game.push_back(kSoundEffect_Cast);
	game.push_back(kSoundEffect_Thwack);

	// use unloadEffect to ensure we don't keep them all in memory always
}

void SoundManager::playEffect( const char *path ) {
#if CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID
	if (path != nullptr) {
		auto instance = CocosDenshion::SimpleAudioEngine::getInstance();
		instance->playEffect( path );
	}
#endif
}

void SoundManager::loader_game(bool in) {
	auto instance = CocosDenshion::SimpleAudioEngine::getInstance();
	for (const char *s : game) {
		if (in)
			instance->preloadEffect(s);
		else
			instance->unloadEffect(s);
	}
}

void SoundManager::startHum() {
	auto instance = CocosDenshion::SimpleAudioEngine::getInstance();
	if (sound_hum > 0)
		instance->stopEffect(sound_hum);
	sound_hum = instance->playEffect(kSoundEffect_Hum, true);
}

void SoundManager::stopHum() {
	auto instance = CocosDenshion::SimpleAudioEngine::getInstance();
	if (sound_hum > 0)
		instance->stopEffect(sound_hum);
}
