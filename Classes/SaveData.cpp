//
//  SaveData.cpp
//  Gems
//
//  Created by Sunil Patel on 25/11/2015.
//
//

#include "SaveData.hpp"

int SaveData::getLevelScore(Level *level) {
	char key[10];
	strcpy(key, "X000-score");
	// Level code:
	key[0] = level->zone;
	// Level value:
	sprintf(key + 1, "%03d", level->level);
	printf("Saving key: %s\n", key);
	auto score = UserDefault::getInstance()->getIntegerForKey(key, 0);
	return score;
}

int SaveData::getInfScore() {
	auto score = UserDefault::getInstance()->getIntegerForKey("inf-score", 0);
	return score;
}

void SaveData::setLevelScore(Level *level, int score) {
	char key[10];
	strcpy(key, "X000-score");
	// Level code:
	key[0] = 'X';
	// Level value:
	sprintf(key + 1, "000");
	printf("Saving key: %s\n", key);
	UserDefault::getInstance()->setIntegerForKey(key, score);
	UserDefault::getInstance()->flush();
}

void SaveData::setInfScore(int score) {
	UserDefault::getInstance()->setIntegerForKey("inf-score", score);
	UserDefault::getInstance()->flush();
}