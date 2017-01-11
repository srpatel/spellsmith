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
	auto r = new Round;
	if (stage == 0) {
		r->monsters.push_back(MonsterManager::get()->get("goblin_sword"));
	} else if (stage == 1) {
		r->monsters.push_back(MonsterManager::get()->get("goblin_axe"));
	} else if (stage == 2) {
		r->monsters.push_back(MonsterManager::get()->get("dog"));
		r->monsters.push_back(MonsterManager::get()->get("goblin_sword"));
	} else if (stage == 3) {
		r->monsters.push_back(MonsterManager::get()->get("goblin_sword"));
		r->monsters.push_back(MonsterManager::get()->get("goblin_axe"));
	} else if (stage == 4) {
		r->monsters.push_back(MonsterManager::get()->get("dog"));
		r->monsters.push_back(MonsterManager::get()->get("goblin_axe"));
	} else if (stage == 5) {
		r->monsters.push_back(MonsterManager::get()->get("goblin_axe"));
		r->monsters.push_back(MonsterManager::get()->get("goblin_axe"));
	} else if (stage == 6) {
		r->monsters.push_back(MonsterManager::get()->get("goblin_axe"));
		r->monsters.push_back(MonsterManager::get()->get("goblin_halberd"));
	} else if (stage == 7) {
		r->monsters.push_back(MonsterManager::get()->get("dog"));
		r->monsters.push_back(MonsterManager::get()->get("goblin_halberd"));
	} else if (stage == 8) {
		r->monsters.push_back(MonsterManager::get()->get("goblin_sword"));
		r->monsters.push_back(MonsterManager::get()->get("goblin_axe"));
		r->monsters.push_back(MonsterManager::get()->get("goblin_halberd"));
	} else if (stage == 9) {
		r->monsters.push_back(MonsterManager::get()->get("goblin_axe"));
		r->monsters.push_back(MonsterManager::get()->get("goblin_halberd"));
		r->monsters.push_back(MonsterManager::get()->get("goblin_halberd"));
	} else {
		r->monsters.push_back(MonsterManager::get()->get((rand() % 2 == 0) ? "goblin_halberd" : "dog"));
		r->monsters.push_back(MonsterManager::get()->get("goblin_halberd"));
		r->monsters.push_back(MonsterManager::get()->get("goblin_halberd"));
	}
	return r;
}
