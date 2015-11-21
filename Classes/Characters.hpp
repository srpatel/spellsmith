//
//  Characters.hpp
//  GemGame
//
//  Created by Sunil Patel on 08/11/2015.
//
//

#ifndef Characters_hpp
#define Characters_hpp

#include "Spell.hpp"

#define HEALTH_PER_HEART 10

enum BuffType {
	BARRIER
};

struct Buff {
	BuffType type;
	bool positive;
	Sprite *sprite;
	Sprite *icon;
	int turns; // -1 = forever, n = lasts n more turns
	int charges; // -1 = infinite, n = n charges remaining
	
};

class Character {
public:
	int max_health;
	int health;
	int ui_health;
	Sprite *sprite;
	std::vector<Buff *> buffs;
};

class Enemy : public Character {
};

class Wizard : public Character {
public:
	std::vector<Spell *> inventory;
};

#endif /* Characters_hpp */
