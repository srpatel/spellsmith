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

void LevelManager::init() {
	// Load levels from JSON
	// Forest levels:
	std::string data = FileUtils::getInstance()->getStringFromFile("data/levels.json");
	rapidjson::Document doc;
	doc.Parse<0>(data.c_str());
	
	For (doc.Size()) {
		const rapidjson::Value& json_level = doc[i];
		
		const rapidjson::Value& json_monsters = json_level["monsters"];
		
		auto l = new Level;
		l->level = i;
		
		for (int j = 0; j < json_monsters.Size(); j++) {
			// TODO : Check monster actually exists
			auto monster = MonsterManager::get()->monsters[json_monsters[j].GetString()];
			l->monsters.push_back(monster);
		}
		
		forest_levels.push_back(l);
	}
}