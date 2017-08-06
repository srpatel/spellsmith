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
class RoundDef;

class LevelManager {
public:
	static LevelManager *get();
	LevelManager();
	inline std::vector<RoundDef *> getRoundDefinitions(){ return rounds; }
	RoundDef *generateRound(int stage);
private:
	static LevelManager *instance;
	std::vector<RoundDef *> rounds;
};

struct RoundDef {
	int id = 0;
	bool generated;
	std::string name;
	std::string bg;
	std::vector<std::string> monsters;
	std::vector<std::string> rewards;
	std::string depends;
	float x, y;
};

#endif /* Level_hpp */
