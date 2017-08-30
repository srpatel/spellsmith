#include "SoundManager.hpp"

// iPhone/desktop = wav
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#define EXTENSION ".ogg"
#elif CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID
#define EXTENSION ".wav"
#endif

const char *kSoundEffect_None        = nullptr;

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
const char *kSoundEffect_PHIce      = "sound/projectile_hit_ice" EXTENSION;
const char *kSoundEffect_PTravel     = "sound/projectile_travel" EXTENSION;
const char *kSoundEffect_Thwack      = "sound/weapon_hit" EXTENSION;
const char *kSoundEffect_Bite       = "sound/animal_bite" EXTENSION;
const char *kSoundEffect_Swipe       = "sound/animal_swipe" EXTENSION;
const char *kSoundEffect_HD1         = "sound/humanoid_death_01" EXTENSION;
const char *kSoundEffect_HD2         = "sound/humanoid_death_02" EXTENSION;
const char *kSoundEffect_HD3         = "sound/humanoid_death_03" EXTENSION;
const char *kSoundEffect_HD4         = "sound/humanoid_death_04" EXTENSION;
const char *kSoundEffect_AD1         = "sound/animal_death_01" EXTENSION;
const char *kSoundEffect_AD2         = "sound/animal_death_02" EXTENSION;

const char *kSoundEffect_UISelect      = "sound/ui_select" EXTENSION;
const char *kSoundEffect_UISelectMinor = "sound/ui_select_minor" EXTENSION;
const char *kSoundEffect_UIBack        = "sound/ui_back" EXTENSION;

const char *kSoundEffect_SPhase      = "sound/spell_phase" EXTENSION;
const char *kSoundEffect_SUnphase    = "sound/spell_unphase" EXTENSION;
const char *kSoundEffect_SRainbow    = "sound/spell_rainbow" EXTENSION;
const char *kSoundEffect_SHeal       = "sound/spell_heal" EXTENSION;
const char *kSoundEffect_SRumble     = "sound/spell_rumble" EXTENSION;
const char *kSoundEffect_SZap        = "sound/spell_zap" EXTENSION;
const char *kSoundEffect_SWhistle    = "sound/dart_whistle_hit" EXTENSION;

SoundManager *SoundManager::instance = nullptr;

SoundManager *SoundManager::get() {
	if (instance == nullptr) {
		instance = new SoundManager;
	}
	return instance;
}

void SoundManager::toggleMute() {
	setMute(! getMute());
}

bool SoundManager::getMute() {
	return oldVolume <= 0;
}

void SoundManager::setMute(bool mute) {
	if (mute) {
		oldVolume = CocosDenshion::SimpleAudioEngine::getInstance()->getEffectsVolume();
		setVolume(0);
	} else {
		setVolume(oldVolume);
		oldVolume = 0;
	}
}

void SoundManager::setVolume(float volume) {
	CocosDenshion::SimpleAudioEngine::getInstance()->setEffectsVolume(volume);
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
	game.push_back(kSoundEffect_PHIce);
	game.push_back(kSoundEffect_PTravel);
	game.push_back(kSoundEffect_SelectGem);
	game.push_back(kSoundEffect_DeselectGem);
	game.push_back(kSoundEffect_Hum);
	game.push_back(kSoundEffect_Fizzle);
	game.push_back(kSoundEffect_Cast);
	game.push_back(kSoundEffect_Thwack);
	game.push_back(kSoundEffect_HD1);
	game.push_back(kSoundEffect_HD2);
	game.push_back(kSoundEffect_HD3);
	game.push_back(kSoundEffect_HD4);
	game.push_back(kSoundEffect_AD1);
	game.push_back(kSoundEffect_AD2);
	
	game.push_back(kSoundEffect_SPhase);
	game.push_back(kSoundEffect_SUnphase);
	game.push_back(kSoundEffect_SRainbow);
	game.push_back(kSoundEffect_SHeal);
	game.push_back(kSoundEffect_SRumble);
	game.push_back(kSoundEffect_SZap);
	game.push_back(kSoundEffect_SWhistle);

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

void SoundManager::humanoidDeath() {
	// randomly pick one of them!
	int x = rand() % 4;
	if (x == 0) {
		PLAY_SOUND(kSoundEffect_HD1);
	} else if (x == 1) {
		PLAY_SOUND(kSoundEffect_HD2);
	} else if (x == 2) {
		PLAY_SOUND(kSoundEffect_HD3);
	} else {
		PLAY_SOUND(kSoundEffect_HD4);
	}
}

void SoundManager::animalDeath() {
	// randomly pick one of them!
	int x = rand() % 2;
	if (x == 0) {
		PLAY_SOUND(kSoundEffect_AD1);
	} else {
		PLAY_SOUND(kSoundEffect_AD2);
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

void SoundManager::startPTravel() {
	auto instance = CocosDenshion::SimpleAudioEngine::getInstance();
	if (sound_ptravel > 0)
		instance->stopEffect(sound_ptravel);
	sound_ptravel = instance->playEffect(kSoundEffect_PTravel, true);
}

void SoundManager::stopPTravel() {
	auto instance = CocosDenshion::SimpleAudioEngine::getInstance();
	if (sound_ptravel > 0)
		instance->stopEffect(sound_ptravel);
}
