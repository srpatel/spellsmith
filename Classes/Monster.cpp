//
//  Monster.cpp
//  Gems
//
//  Created by Sunil Patel on 26/11/2015.
//
//

#include "Monster.hpp"

#include "json/document.h"

MonsterManager *MonsterManager::instance = nullptr;

MonsterManager *MonsterManager::get() {
	if (instance == nullptr) {
		instance = new MonsterManager;
	}
	return instance;
}

static AttackType getAttackType(std::string s) {
	return kAttackTypeMelee;
}

void MonsterManager::init() {
	// Load monsters from JSON
	std::string data = FileUtils::getInstance()->getStringFromFile("data/monsters.json");
	rapidjson::Document doc;
	doc.Parse<0>(data.c_str());
	
	For (doc.Size()) {
		const rapidjson::Value& json_monster = doc[i];
		
		const rapidjson::Value& json_name = json_monster["name"];
		const rapidjson::Value& json_health = json_monster["health"];
		const rapidjson::Value& json_attacks = json_monster["attacks"];
		
		std::string name = json_name.GetString();
		int health = json_health.GetInt();
		
		auto m = new Monster;
		m->hp = health;

		for (int j = json_attacks.Size() - 1; j >= 0; j--) {
			const rapidjson::Value& json_attack = json_attacks[i];
			const rapidjson::Value& json_frequency = json_attack["freq"];
			const rapidjson::Value& json_type = json_attack["type"];
			const rapidjson::Value& json_amount = json_attack["amount"];
			
			auto type = getAttackType(json_type.GetString());
			
			auto attack = new Attack(
				json_frequency.GetInt(),
				type,
				json_amount.GetInt()
			);
			m->attacks.push_back(attack);
		}
		
		monsters[name] = m;
	}
}

Attack::Attack(int _frequency, AttackType _type, int _amount) :
	frequency(_frequency),
	type(_type),
	amount(_amount)
{}