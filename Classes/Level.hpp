//
//  Level.hpp
//  Gems
//
//  Created by Sunil Patel on 26/11/2015.
//
//

#ifndef Level_hpp
#define Level_hpp

const char kZoneForest = 'F';
const char kZoneLaboratory = 'L';

class Level {
	friend class LevelManager;
public:
	char zone = kZoneForest;
	int level = 0;
	
	// monsters...
};

class LevelManager {
public:
	static LevelManager *get();
	void init();
private:
	static LevelManager *instance;
};

#endif /* Level_hpp */
