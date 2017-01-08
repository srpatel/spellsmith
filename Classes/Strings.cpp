//
//  Strings.cpp
//  GemGame
//
//  Created by Sunil Patel on 07/11/2015.
//
//

#include "Strings.hpp"

Strings *Strings::get() {
	static Strings *instance = nullptr;
	if (instance == nullptr) {
		instance = new Strings;
	}
	return instance;
}

void Strings::init() {
	// Load JSON stuff!
	// TODO : Load correct language
	std::string data = FileUtils::getInstance()->getStringFromFile("data/strings/en.json");
	rapidjson::Document doc;
	doc.Parse<0>(data.c_str());
	
	if (doc.GetType() != rapidjson::Type::kObjectType) {
		LOG("Could not parse JSON!");
	}
	// Load elements at root level (no key) - other files might have starter key?
	loadObject("", doc);
}

void Strings::loadObject(std::string key, const rapidjson::Value& value) {
	for (rapidjson::Value::ConstMemberIterator itr = value.MemberBegin(); itr != value.MemberEnd(); ++itr) {
		std::string currentkey = key + itr->name.GetString();
		if (itr->value.GetType() == rapidjson::Type::kObjectType) {
			// recursive
			loadObject(currentkey + ".", itr->value);
		} else {
			// load strings
			table[currentkey] = itr->value.GetString();
		}
	}
}

std::string Strings::translate(std::string key) {
	auto t = table.find(key);
	if (t == table.end()) {
		printf("Translation missing: %s\n", key.c_str());
		return key;
	} else {
		return t->second;
	}
}
