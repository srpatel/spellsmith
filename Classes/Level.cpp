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

LevelManager::LevelManager() {
	std::string data = FileUtils::getInstance()->getStringFromFile("data/levels.json");
	rapidjson::Document doc;
	doc.Parse<0>(data.c_str());
	
	if (doc.GetType() != rapidjson::Type::kObjectType) {
		LOG("Could not parse JSON!");
	}
	
	for (rapidjson::Value::ConstMemberIterator itr = doc.MemberBegin(); itr != doc.MemberEnd(); ++itr) {
		std::string currentkey = itr->name.GetString();
		//
		RoundDef *r = new RoundDef;
		r->name = currentkey;
		
		const auto &monsters = itr->value["monsters"];
		for (int i = 0; i < monsters.Size(); i++) {
			r->monsters.push_back(monsters[i].GetString());
		}
		
		const auto &rewards = itr->value["rewards"];
		for (int i = 0; i < rewards.Size(); i++) {
			r->rewards.push_back(rewards[i].GetString());
		}
		
		r->x = itr->value["x"].GetDouble();
		r->y = itr->value["y"].GetDouble();
		r->depends = itr->value["depends"].GetString();
		r->generated = false;
		
		rounds.push_back(r);
	}
}

LevelManager *LevelManager::get() {
	if (instance == nullptr) {
		instance = new LevelManager;
	}
	return  instance;
}

RoundDef *LevelManager::generateRound(int stage) {
	auto r = new RoundDef;
	r->generated = true;
	if (stage == 0) {
		r->monsters.push_back("goblin_sword");
	} else if (stage <= 2) {
		// Just one enemy!
		if (rand() % 2) {
			r->monsters.push_back(("goblin_axe"));
		} else {
			r->monsters.push_back(("goblin_halberd"));
		}
	} else if (stage <= 5) {
		int difficulty = rand() % 3;
		
		if (difficulty == 0) {
			r->monsters.push_back(("goblin_sword"));
			r->monsters.push_back(("goblin_axe"));
		} else if (difficulty == 1) {
			r->monsters.push_back(("goblin_sword"));
			r->monsters.push_back(("goblin_sword"));
		} else {
			r->monsters.push_back(("goblin_sword"));
			r->monsters.push_back(("goblin_halberd"));
		}
	} else if (stage <= 10) {
		int difficulty = rand() % 9;
		if (difficulty <= 2) {
			r->monsters.push_back(("shaman"));
			r->monsters.push_back(("dog"));
			if (difficulty == 1) {
				r->monsters.push_back(("fast_dog"));
			} else if (difficulty == 2) {
				r->monsters.push_back(("dog"));
			}
		} else if (difficulty <= 4) {
			if (rand() % 2 == 0) {
				r->monsters.push_back(("goblin_sword"));
			}
			if (difficulty == 4) {
				r->monsters.push_back(("goblin_sword"));
			} else if (difficulty == 3) {
				r->monsters.push_back(("goblin_halberd"));
			}
			r->monsters.push_back(("dog"));
		} else if (difficulty <= 6) {
			if (rand() % 2 == 0) {
				r->monsters.push_back(("fire_wizard"));
			} else {
				r->monsters.push_back(("water_wizard"));
			}
			if (difficulty == 4) {
				r->monsters.push_back(("goblin_sword"));
			} else if (difficulty == 3) {
				r->monsters.push_back(("goblin_halberd"));
			}
			r->monsters.push_back(("goblin_sword"));
		} else if (difficulty <= 7) {
			r->monsters.push_back(("water_wizard"));
			r->monsters.push_back(("fire_wizard"));
		} else {
			r->monsters.push_back(("goblin_sword"));
			if (difficulty == 4) {
				r->monsters.push_back(("goblin_sword"));
			} else if (difficulty == 3) {
				r->monsters.push_back(("goblin_axe"));
			} else if (difficulty == 2) {
				r->monsters.push_back(("goblin_halberd"));
			}
			r->monsters.push_back(((rand() % 2 == 0) ? "goblin_halberd" : "fast_dog"));
		}
	} else {
		if (rand() % 2 == 0) {
			r->monsters.push_back(((rand() % 2 == 0) ? "fast_dog" : "dog"));
			r->monsters.push_back(("shaman"));
			r->monsters.push_back(((rand() % 2 == 0) ? "fast_dog" : "dog"));
		} else {
			r->monsters.push_back(((rand() % 2 == 0) ? "goblin_halberd" : "dog"));
			r->monsters.push_back(((rand() % 2 == 0) ? "fire_wizard" : "water_wizard"));
			r->monsters.push_back(((rand() % 2 == 0) ? "goblin_halberd" : "fast_dog"));
		}
	}
	if (r->monsters.size() > 3) {
		printf("Shouldn't happen. Remove the extra monsters.");
		r->monsters.erase(r->monsters.end() - 3, r->monsters.end());
	}
	std::random_shuffle(r->monsters.begin(), r->monsters.end());
	return r;
}
