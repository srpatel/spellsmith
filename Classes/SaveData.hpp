//
//  SaveData.hpp
//  Gems
//
//  Created by Sunil Patel on 25/11/2015.
//
//

#ifndef SaveData_hpp
#define SaveData_hpp

#include "Level.hpp"

struct SaveData {
	// Get level scores
	static int getArenaScore();
	static void setArenaScore(int);
	
	// get level completion
	static int isLevelComplete(std::string name); /* returns number of moves */
	static void setLevelComplete(std::string name, int moves /* 0 = incomplete */);
	
	// get spell list!
	static std::vector<std::string> getSpells();
	static void addSpell(std::string name);
};

#endif /* SaveData_hpp */
