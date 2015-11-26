//
//  Level.cpp
//  Gems
//
//  Created by Sunil Patel on 26/11/2015.
//
//

#include "Level.hpp"

LevelManager *LevelManager::instance = nullptr;

LevelManager *LevelManager::get() {
	if (instance == nullptr) {
		instance = new LevelManager;
	}
	return  instance;
}

void LevelManager::init() {
	// Load levels from JSON
}