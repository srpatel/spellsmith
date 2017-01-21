//
//  SoundManager.hpp
//  Gems
//
//  Created by Sunil Patel on 08/01/2017.
//
//

#ifndef SoundManager_hpp
#define SoundManager_hpp

#include "SimpleAudioEngine.h"

#define PLAY_SOUND(_s_) SoundManager::get()->playEffect( _s_ )

extern const char
	*kSoundEffect_SelectGem,
	*kSoundEffect_DeselectGem,
	*kSoundEffect_Hum,
	*kSoundEffect_Fizzle,
	*kSoundEffect_Cast,
	*kSoundEffect_LevelWin,
	*kSoundEffect_LevelLose,
	*kSoundEffect_PHAir,
	*kSoundEffect_PHWater,
	*kSoundEffect_PHEarth,
	*kSoundEffect_PHFire,
	*kSoundEffect_UISelect,
	*kSoundEffect_UISelectMinor,
	*kSoundEffect_UIBack,
	*kSoundEffect_Thwack,
	*kSoundEffect_PTravel,
	*kSoundEffect_HD1,
	*kSoundEffect_HD2,
	*kSoundEffect_HD3,
	*kSoundEffect_HD4,
	*kSoundEffect_AD1,
	*kSoundEffect_AD2;

class SoundManager {
public:
	static SoundManager *get();
	void init();
	void playEffect( const char *path );
	void startHum();
	void stopHum();
	void startPTravel();
	void stopPTravel();
	void humanoidDeath();
	void animalDeath();
	
	void loader_game(bool in);
private:
	static SoundManager *instance;
	unsigned int sound_hum = -1;
	unsigned int sound_ptravel = -1;
	std::vector<const char *> game;
};

#endif /* SoundManager_hpp */
