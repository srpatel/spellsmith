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

class Character {
public:
	int max_health;
	int health;
};

class Enemy : public Character {
};

class Wizard : public Character {
public:
	std::vector<Spell *> inventory;
};

#endif /* Characters_hpp */
