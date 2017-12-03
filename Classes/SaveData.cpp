//
//  SaveData.cpp
//  Gems
//
//  Created by Sunil Patel on 25/11/2015.
//
//

#include "SaveData.hpp"
#include "Level.hpp"

#define kArenaStateVersion 6

static std::string GetArenaKey(int version = kArenaStateVersion) {
	return std::string{"arena-state"} + ToString(version);
}

std::string SaveData::getUuid() {
	static std::string uuid = {};
	if (uuid.empty()) {
		auto ud = UserDefault::getInstance();
		uuid = ud->getStringForKey("uuid", "");
		if (uuid.empty()) {
			char buffer[33] = {0};
			const char charset[] =
				"0123456789"
				"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
				"abcdefghijklmnopqrstuvwxyz";
			for (int i = 0; i < 32; ++i) {
				buffer[i] = charset[rand() % (sizeof(charset) - 1)];
			}
			uuid = buffer;
			ud->setStringForKey("uuid", uuid);
		}
	}
	return uuid;
}

void SaveData::clear() {
	auto ud = UserDefault::getInstance();
	// TODO - finish...
	For(6) {
		char key[50];
		sprintf(key, "spell-equipped-%d", i);
		ud->deleteValueForKey(key);
	}
	
	ud->deleteValueForKey("inf-score");
	ud->deleteValueForKey(GetArenaKey().c_str());
	ud->deleteValueForKey("spell-inventory");
	
	for (RoundDef *rd : LevelManager::get()->getRoundDefinitions()) {
		std::string key = std::string("level-moves-") + rd->name;
		ud->deleteValueForKey(key.c_str());
	}
	
	ud->flush();
}

int SaveData::getArenaScore() {
	auto score = UserDefault::getInstance()->getIntegerForKey("inf-score", 0);
	return score;
}

void SaveData::setArenaState(std::string state) {
	UserDefault::getInstance()->setStringForKey(GetArenaKey().c_str(), state);
}

std::string SaveData::getArenaState() {
	// Clear all old arena states
	/*for (int i = 0; i < kArenaStateVersion; i++) {
		UserDefault::getInstance()->deleteValueForKey(GetArenaKey(i).c_str());
	}*/
	return UserDefault::getInstance()->getStringForKey(GetArenaKey().c_str(), "");
}

void SaveData::setArenaScore(int score) {
	if (score <= getArenaScore()) return;
	UserDefault::getInstance()->setIntegerForKey("inf-score", score);
	UserDefault::getInstance()->flush();
}

int SaveData::isLevelComplete(std::string name) {
	std::string key = std::string("level-moves-") + name;
	auto moves = UserDefault::getInstance()->getIntegerForKey(key.c_str(), 0);
	return moves;
}

void SaveData::setLevelComplete(std::string name, int moves) {
	std::string key = std::string("level-moves-") + name;
	UserDefault::getInstance()->setIntegerForKey(key.c_str(), moves);
	UserDefault::getInstance()->flush();
}

std::string SaveData::getEquippedSpellAt(int i) {
	char key[50];
	sprintf(key, "spell-equipped-%d", i);
	return UserDefault::getInstance()->getStringForKey(key, "");
}

void SaveData::setEquippedSpellAt(int j, std::string name) {
	// If the spell is already equipped, then unequip it from elsewhere
	if (!name.empty()) {
		For (6) {
			if (name == SaveData::getEquippedSpellAt(i)) {
				SaveData::setEquippedSpellAt(i, "");
			}
		}
	}
	char key[50];
	sprintf(key, "spell-equipped-%d", j);
	UserDefault::getInstance()->setStringForKey(key, name);
	UserDefault::getInstance()->flush();
}

#if DESKTOP && 0
	#include "Spell.hpp"
#endif

std::vector<std::string> SaveData::getSpells() {
#if DESKTOP && 0
	std::vector<std::string> result;
	for (Spell *spell : SpellManager::get()->spells)
		result.push_back(spell->getRawName());
	return result;
#else
	std::string list = UserDefault::getInstance()->getStringForKey("spell-inventory", "");
	
	std::vector<std::string> result;
	
	auto begin = list.find_first_not_of(",");
	auto end = 0;
	while ((end = list.find_first_of(",", begin)) != std::string::npos) {
		result.push_back(list.substr(begin, end - begin));
        begin = list.find_first_not_of(",", end);
	}
	if(begin != std::string::npos) {
        result.push_back(list.substr(begin));
	}
	return result;
#endif
}

bool SaveData::addSpell(std::string name) {
	auto spells = getSpells();
	
	// If the spell doesn't exist
	if (std::find(spells.begin(), spells.end(), name) == spells.end()) {
		spells.push_back(name);
		std::string list;
		for (std::string s : spells) {
			if (! list.empty()) {
				list += ",";
			}
			list += s;
		}
		UserDefault::getInstance()->setStringForKey("spell-inventory", list);
		UserDefault::getInstance()->flush();
		return true;
	} else {
		printf("Already know that spell.\n");
		return false;
	}
}
