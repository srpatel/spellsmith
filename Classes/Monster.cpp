//
//  Monster.cpp
//  Gems
//
//  Created by Sunil Patel on 26/11/2015.
//
//

#include "Monster.hpp"

MonsterManager *MonsterManager::instance = nullptr;

MonsterManager *MonsterManager::get() {
	if (instance == nullptr) {
		instance = new MonsterManager;
	}
	return instance;
}

void MonsterManager::init() {
	// Load monsters from JSON
}