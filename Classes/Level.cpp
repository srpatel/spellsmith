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
	} else if (stage <= 2) {
		// Just one enemy!
		if (rand() % 2) {
			r->monsters.push_back(MonsterManager::get()->get("goblin_axe"));
		} else {
			r->monsters.push_back(MonsterManager::get()->get("goblin_halberd"));
		}
	} else if (stage <= 5) {
		int difficulty = rand() % 3;
		
		if (difficulty == 0) {
			r->monsters.push_back(MonsterManager::get()->get("goblin_sword"));
			r->monsters.push_back(MonsterManager::get()->get("goblin_axe"));
		} else if (difficulty == 1) {
			r->monsters.push_back(MonsterManager::get()->get("goblin_sword"));
			r->monsters.push_back(MonsterManager::get()->get("goblin_sword"));
		} else {
			r->monsters.push_back(MonsterManager::get()->get("goblin_halberd"));
		}
	} else if (stage <= 10) {
		int difficulty = rand() % 9;
		if (difficulty <= 2) {
			r->monsters.push_back(MonsterManager::get()->get("shaman"));
			r->monsters.push_back(MonsterManager::get()->get("dog"));
			if (difficulty == 1) {
				r->monsters.push_back(MonsterManager::get()->get("fast_dog"));
			} else if (difficulty == 2) {
				r->monsters.push_back(MonsterManager::get()->get("dog"));
			}
		} else if (difficulty <= 4) {
			if (rand() % 2 == 0) {
				r->monsters.push_back(MonsterManager::get()->get("goblin_sword"));
			}
			if (difficulty == 4) {
				r->monsters.push_back(MonsterManager::get()->get("goblin_sword"));
			} else if (difficulty == 3) {
				r->monsters.push_back(MonsterManager::get()->get("goblin_halberd"));
			}
			r->monsters.push_back(MonsterManager::get()->get("dog"));
		} else if (difficulty <= 6) {
			if (rand() % 2 == 0) {
				r->monsters.push_back(MonsterManager::get()->get("fire_wizard"));
			} else {
				r->monsters.push_back(MonsterManager::get()->get("water_wizard"));
			}
			if (difficulty == 4) {
				r->monsters.push_back(MonsterManager::get()->get("goblin_sword"));
			} else if (difficulty == 3) {
				r->monsters.push_back(MonsterManager::get()->get("goblin_halberd"));
			}
			r->monsters.push_back(MonsterManager::get()->get("goblin_sword"));
		} else if (difficulty <= 7) {
			r->monsters.push_back(MonsterManager::get()->get("water_wizard"));
			r->monsters.push_back(MonsterManager::get()->get("fire_wizard"));
		} else {
			r->monsters.push_back(MonsterManager::get()->get("goblin_sword"));
			if (difficulty == 4) {
				r->monsters.push_back(MonsterManager::get()->get("goblin_sword"));
			} else if (difficulty == 3) {
				r->monsters.push_back(MonsterManager::get()->get("goblin_axe"));
			} else if (difficulty == 2) {
				r->monsters.push_back(MonsterManager::get()->get("goblin_halberd"));
			}
			r->monsters.push_back(MonsterManager::get()->get((rand() % 2 == 0) ? "goblin_halberd" : "fast_dog"));
		}
	} else {
		r->monsters.push_back(MonsterManager::get()->get((rand() % 2 == 0) ? "goblin_halberd" : "dog"));
		r->monsters.push_back(MonsterManager::get()->get((rand() % 2 == 0) ? "goblin_halberd" : "shaman"));
		r->monsters.push_back(MonsterManager::get()->get((rand() % 2 == 0) ? "goblin_halberd" : "fast_dog"));
	}
	if (r->monsters.size() > 3) {
		printf("Shouldn't happen. Remove the extra monsters.");
		r->monsters.erase(r->monsters.end() - 3, r->monsters.end());
	}
	std::random_shuffle(r->monsters.begin(), r->monsters.end());
	return r;
}
