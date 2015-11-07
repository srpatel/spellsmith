//
//  Strings.hpp
//  GemGame
//
//  Created by Sunil Patel on 07/11/2015.
//
//

#ifndef Strings_hpp
#define Strings_hpp

#include "json/document.h"

// Translation shortcut: _("string")
#define _(_x_) Strings::get()->translate((_x_))

class Strings {
public:
	static Strings *get();
	void init();
	std::string translate(std::string);
private:
	std::unordered_map<std::string, std::string> table;
	void loadObject(std::string key, const rapidjson::Value& value);
};

#endif /* Strings_hpp */
