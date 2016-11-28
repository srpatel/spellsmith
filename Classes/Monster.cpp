//
//  Monster.cpp
//  Gems
//
//  Created by Sunil Patel on 26/11/2015.
//
//

#include "Monster.hpp"
#include "ImageManager.hpp"

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

Monster *MonsterManager::get(std::string name) {
	auto m = MonsterManager::get()->monsters[name];
	if (m == nullptr) {
		LOG("Monster not found");
	}
	return m;
}

void MonsterManager::init() {
	// Load monsters from JSON
	std::string data = FileUtils::getInstance()->getStringFromFile("data/monsters.json");
	rapidjson::Document doc;
	doc.Parse<0>(data.c_str());
	
	For (doc.Size()) {
		const rapidjson::Value& json_monster = doc[i];
		
		const rapidjson::Value& json_name = json_monster["name"];
		const rapidjson::Value& json_skeleton = json_monster["skeleton"];
		const rapidjson::Value& json_health = json_monster["health"];
		const rapidjson::Value& json_attack_frequency = json_monster["attack_frequency"];
		const rapidjson::Value& json_attacks = json_monster["attacks"];
		
		std::string name = json_name.GetString();
		int health = json_health.GetInt();
		
		auto m = new Monster;
		m->name = name;
		m->hp = health;
		m->skeleton.path = json_skeleton["name"].GetString();
		for (int j = json_skeleton["attachments"].Size() - 1; j >= 0; j--) {
			const rapidjson::Value& a = json_skeleton["attachments"][j];
			auto it = a.MemberBegin();
			while (it != a.MemberEnd()) {
				m->skeleton.attachments[it->name.GetString()] = it->value.GetString();
				it++;
			}
		}
		if (json_skeleton.HasMember("skins")) {
			for (int j = json_skeleton["skins"].Size() - 1; j >= 0; j--) {
				const rapidjson::Value& a = json_skeleton["skins"][j];
				m->skeleton.skins.push_back(a.GetString());
			}
		}
		
		m->attack_frequency = json_attack_frequency.GetInt();
		int total_attack_ratio = 0;
		for (int j = json_attacks.Size() - 1; j >= 0; j--) {
			const rapidjson::Value& json_attack = json_attacks[j];
			const rapidjson::Value& json_ratio = json_attack["ratio"];
			const rapidjson::Value& json_type = json_attack["type"];
			const rapidjson::Value& json_amount = json_attack["amount"];
			const rapidjson::Value& json_animation = json_attack["animation"];
			
			auto type = getAttackType(json_type.GetString());
			int ratio = json_ratio.GetInt();
			
			total_attack_ratio += ratio;
			
			auto attack = new Attack(
				ratio,
				type,
				json_animation.GetString(),
				json_amount.GetInt()
			);
			m->attacks.push_back(attack);
		}
		m->total_attack_ratio = total_attack_ratio;
		monsters[name] = m;
	}
}

spine::SkeletonAnimation *Monster::makeSkeleton() {
	auto path = (std::string("spine/") + skeleton.path + ".json");
	auto boy = spine::SkeletonAnimation::createWithFile(path.c_str(), ImageManager::get()->getAtlas(), 0.25f);
	
	// Set attachments
	for (auto it : skeleton.attachments) {
		boy->setAttachment(it.first, it.second);
	}
	
	// Set skin
	if (skeleton.skins.size() > 0) {
		int i = rand() % skeleton.skins.size();
		boy->setSkin(skeleton.skins[i]);
	}

	return boy;
}

Attack *Monster::getAttack() {
	// pick a random int up to the sum
	int pivot = rand() % total_attack_ratio;
	int current = 0;
	for (Attack *attack : attacks) {
		current += attack->ratio;
		if (pivot < current) {
			return attack;
		}
	}
	// should never happen!
	return attacks[0];
}

Attack::Attack(int _ratio, AttackType _type, std::string _animation, int _amount) :
	ratio(_ratio),
	type(_type),
	amount(_amount),
	animation(_animation)
{}
