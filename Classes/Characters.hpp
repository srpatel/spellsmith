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
#include "Monster.hpp"

enum BuffType {
	BARRIER,
	FREEZE,
	STUN
};

struct Buff {
	BuffType type;
	bool positive;
	Sprite *icon;
	Sprite *sprite;
	int turns; // -1 = forever, n = lasts n more turns
	int charges; // -1 = infinite, n = n charges remaining
	
	int priority;
	
	static Buff *createMudshield();
	static Buff *createFreeze(int amount);
	static Buff *createStun();
	~Buff();
};

bool BuffComparator (Buff *left, Buff *right);

class Character {
public:
	int max_health;
	int health;
	int ui_health;
	Sprite *sprite;
	std::vector<Buff *> buffs;
	Buff *getBuffByType(BuffType);
	void addBuff(Buff *);
	void removeBuff(Buff *);
	inline bool dead() { return health <= 0; }
};

class Enemy : public Character {
public:
	Monster *monster;
	int index;
	int attack_clock;
	Enemy(Monster *, int index);
	virtual ~Enemy();
private:
};

class Wizard : public Character {
public:
	std::vector<Spell *> inventory;
};

#endif /* Characters_hpp */
