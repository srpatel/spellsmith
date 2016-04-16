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

class Round;

class LevelManager {
public:
	static LevelManager *get();
	Round *generateRound(int stage);
private:
	static LevelManager *instance;
};

class Round {
	// For now, just a list of monsters.
	std::vector<Monster *> monsters;
};

class SaveGame {
	// For now - does nothing!
};

#endif /* Level_hpp */
