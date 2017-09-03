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

std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

std::string Strings::translate(std::string key, std::initializer_list<std::string> replacements) {
	auto t = table.find(key);
	if (t == table.end()) {
		printf("Translation missing: %s\n", key.c_str());
		return key;
	} else {
		if (replacements.size() > 0) {
			std::string copy {t->second};
			int i = 1;
			for (auto to : replacements) {
				std::string from = std::string("{") + ToString(i) + "}";
				size_t start_pos = 0;
				while((start_pos = copy.find(from, start_pos)) != std::string::npos) {
					copy.replace(start_pos, from.length(), to);
					start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
				}
				i++;
			}
			return copy;
		} else {
			// Don't bother returning a copy
			return t->second;
		}
	}
}
