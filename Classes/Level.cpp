//
//  Level.cpp
//  Gems
//
//  Created by Sunil Patel on 26/11/2015.
//
//

#include "Level.hpp"

#include "json/document.h"

LevelManager *LevelManager::instance = nullptr;

LevelManager *LevelManager::get() {
	if (instance == nullptr) {
		instance = new LevelManager;
	}
	return  instance;
}

Round *LevelManager::generateRound(int stage) {
	// TODO : Garbo this!
	auto r = new Round;
	r->monsters.push_back(MonsterManager::get()->get("goblin_halberd"));
	r->monsters.push_back(MonsterManager::get()->get("goblin_sword"));
	return r;
}