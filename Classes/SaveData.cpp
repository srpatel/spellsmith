//
//  SaveData.cpp
//  Gems
//
//  Created by Sunil Patel on 25/11/2015.
//
//

#include "SaveData.hpp"

int SaveData::getInfScore() {
	auto score = UserDefault::getInstance()->getIntegerForKey("inf-score", 0);
	return score;
}

void SaveData::setInfScore(int score) {
	UserDefault::getInstance()->setIntegerForKey("inf-score", score);
	UserDefault::getInstance()->flush();
}