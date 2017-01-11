//
//  SoundManager.hpp
//  Gems
//
//  Created by Sunil Patel on 08/01/2017.
//
//

#ifndef SoundManager_hpp
#define SoundManager_hpp

extern const char
	*kSoundEffect_SelectGem,
	*kSoundEffect_DeselectGem,
	*kSoundEffect_CastSpell,
	*kSoundEffect_Fizzle,
	*kSoundEffect_Click;

class SoundManager {
public:
	static SoundManager *get();
	void init();
	void playEffect( const char *path );
private:
	static SoundManager *instance;
};

#endif /* SoundManager_hpp */
