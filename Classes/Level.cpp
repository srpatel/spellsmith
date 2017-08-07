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
	int i = 0;
	for (rapidjson::Value::ConstMemberIterator itr = doc.MemberBegin(); itr != doc.MemberEnd(); ++itr) {
		std::string currentkey = itr->name.GetString();
		//
		RoundDef *r = new RoundDef;
		r->id = i++;
		r->name = currentkey;
		r->bg = std::string("bg_") + itr->value["bg"].GetString() + ".png";
		
		const auto &monsters = itr->value["monsters"];
		for (int i = 0; i < monsters.Size(); i++) {
			std::vector<std::string> wave;
			for (int j = 0; j < monsters[i].Size(); j++) {
				wave.push_back(monsters[i][j].GetString());
			}
			r->waves.push_back(wave);
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
	r->id = stage;
	r->generated = true;
	std::vector<std::string> wave;
	if (stage == 0) {
		wave.push_back("goblin_sword");
	} else if (stage <= 2) {
		// Just one enemy!
		if (rand() % 2) {
			wave.push_back(("goblin_axe"));
		} else {
			wave.push_back(("goblin_halberd"));
		}
	} else if (stage <= 5) {
		int difficulty = rand() % 3;
		
		if (difficulty == 0) {
			wave.push_back(("goblin_sword"));
			wave.push_back(("goblin_axe"));
		} else if (difficulty == 1) {
			wave.push_back(("goblin_sword"));
			wave.push_back(("goblin_sword"));
		} else {
			wave.push_back(("goblin_sword"));
			wave.push_back(("goblin_halberd"));
		}
	} else if (stage <= 10) {
		int difficulty = rand() % 9;
		if (difficulty <= 2) {
			wave.push_back(("shaman"));
			wave.push_back(("dog"));
			if (difficulty == 1) {
				wave.push_back(("fast_dog"));
			} else if (difficulty == 2) {
				wave.push_back(("dog"));
			}
		} else if (difficulty <= 4) {
			if (rand() % 2 == 0) {
				wave.push_back(("goblin_sword"));
			}
			if (difficulty == 4) {
				wave.push_back(("goblin_sword"));
			} else if (difficulty == 3) {
				wave.push_back(("goblin_halberd"));
			}
			wave.push_back(("dog"));
		} else if (difficulty <= 6) {
			if (rand() % 2 == 0) {
				wave.push_back(("fire_wizard"));
			} else {
				wave.push_back(("water_wizard"));
			}
			if (difficulty == 4) {
				wave.push_back(("goblin_sword"));
			} else if (difficulty == 3) {
				wave.push_back(("goblin_halberd"));
			}
			wave.push_back(("goblin_sword"));
		} else if (difficulty <= 7) {
			wave.push_back(("water_wizard"));
			wave.push_back(("fire_wizard"));
		} else {
			wave.push_back(("goblin_sword"));
			if (difficulty == 4) {
				wave.push_back(("goblin_sword"));
			} else if (difficulty == 3) {
				wave.push_back(("goblin_axe"));
			} else if (difficulty == 2) {
				wave.push_back(("goblin_halberd"));
			}
			wave.push_back(((rand() % 2 == 0) ? "goblin_halberd" : "fast_dog"));
		}
	} else {
		if (rand() % 2 == 0) {
			wave.push_back(((rand() % 2 == 0) ? "fast_dog" : "dog"));
			wave.push_back(("shaman"));
			wave.push_back(((rand() % 2 == 0) ? "fast_dog" : "dog"));
		} else {
			wave.push_back(((rand() % 2 == 0) ? "goblin_halberd" : "dog"));
			wave.push_back(((rand() % 2 == 0) ? "fire_wizard" : "water_wizard"));
			wave.push_back(((rand() % 2 == 0) ? "goblin_halberd" : "fast_dog"));
		}
	}
	if (wave.size() > 3) {
		printf("Shouldn't happen. Remove the extra monsters.");
		wave.erase(wave.end() - 3, wave.end());
	}
	std::random_shuffle(wave.begin(), wave.end());
	r->waves.push_back(wave);
	return r;
}
