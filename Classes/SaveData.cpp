//
//  SaveData.cpp
//  Gems
//
//  Created by Sunil Patel on 25/11/2015.
//
//

#include "SaveData.hpp"

int SaveData::getArenaScore() {
	auto score = UserDefault::getInstance()->getIntegerForKey("inf-score", 0);
	return score;
}

void SaveData::setArenaScore(int score) {
	UserDefault::getInstance()->setIntegerForKey("inf-score", score);
	UserDefault::getInstance()->flush();
}

int SaveData::isLevelComplete(std::string name) {
	std::string key = std::string("level-moves-") + name;
	auto moves = UserDefault::getInstance()->getIntegerForKey(key.c_str(), 0);
	return moves;
}

void SaveData::setLevelComplete(std::string name, int moves) {
	std::string key = std::string("level-moves-") + name;
	UserDefault::getInstance()->setIntegerForKey(key.c_str(), moves);
	UserDefault::getInstance()->flush();
}

std::vector<std::string> SaveData::getSpells() {
	std::string list = UserDefault::getInstance()->getStringForKey("spell-inventory", "");
	
	std::vector<std::string> result;
	
	auto begin = list.find_first_not_of(",");
	auto end = 0;
	while ((end = list.find_first_of(",", begin)) != std::string::npos) {
		result.push_back(list.substr(begin, end - begin));
        begin = list.find_first_not_of(",", end);
	}
	if(begin != std::string::npos) {
        result.push_back(list.substr(begin));
	}
	return result;
}

void SaveData::addSpell(std::string name) {
	auto spells = getSpells();
	
	// If the spell doesn't exist
	if (std::find(spells.begin(), spells.end(), name) == spells.end()) {
		spells.push_back(name);
		std::string list;
		for (std::string s : spells) {
			if (! list.empty()) {
				list += ",";
			}
			list += s;
		}
		UserDefault::getInstance()->setStringForKey("spell-inventory", list);
		UserDefault::getInstance()->flush();
	} else {
		printf("Already know that spell.\n");
	}
}
