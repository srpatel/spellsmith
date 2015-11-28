//
//  Level.hpp
//  Gems
//
//  Created by Sunil Patel on 26/11/2015.
//
//

#ifndef Level_hpp
#define Level_hpp

#include "Monster.hpp"

const char kZoneForest = 'F';
const char kZoneLaboratory = 'L';

class Level {
	friend class LevelManager;
public:
	char zone = kZoneForest;
	int level = 0;
	std::vector<Monster *> monsters;
	// monsters...
};

class LevelManager {
public:
	static LevelManager *get();
	void init();
	std::vector<Level *> forest_levels;
	//std::vector<Level *> laboratory_levels; etc.
private:
	static LevelManager *instance;
};

#endif /* Level_hpp */
