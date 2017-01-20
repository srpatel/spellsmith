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
	BARRIER = 1,     // UNUSED!
	FREEZE,
	STUN,
	FURY,
	KINGS_COURT, // UNUSED!
	PHASING,
	FOCUS,
	CHARGE_BOLT
};

struct Buff {
	BuffType type;
	bool positive;
	std::string icon;
	Sprite *sprite;
	int turns; // -1 = forever, n = lasts n more turns
	int charges; // -1 = infinite, n = n charges remaining
	
	int priority;
	
	static Buff *createMudshield();
	static Buff *createFreeze(int amount);
	static Buff *createStun();
	static Buff *createFury();
	static Buff *createKingsCourt();
	static Buff *createPhasing();
	static Buff *createFocus();
	static Buff *createChargeBolt();
	~Buff();
};

bool BuffComparator (Buff *left, Buff *right);

class Character {
public:
	int max_health;
	int health;
	int ui_health;
	bool is_skeleton = false;
	float projectile_height;
	Node *sprite;
	std::vector<Buff *> buffs;
	Buff *getBuffByType(BuffType);
	void addBuff(Buff *);
	void removeBuff(Buff *);
	void clearBuffs();
	void updateBuffs();
	void tickBuffs();
	inline bool dead() { return health <= 0; }
	Layer *buffHolder;
	void flash(Color3B c);
	void heal(int amt);
	void damageEffect(int damage);
	virtual std::map<std::string, Vec2> getOffsets() = 0;
};

class Enemy : public Character {
public:
	Monster *monster;
	int index;
	int attack_clock;
	Enemy(Monster *, int index);
	virtual ~Enemy();
	virtual std::map<std::string, Vec2> getOffsets();
private:
};

class Wizard : public Character {
public:
	Wizard();
	std::vector<Spell *> inventory;
	virtual std::map<std::string, Vec2> getOffsets();
private:
	std::map<std::string, Vec2> offsets;
};

#endif /* Characters_hpp */
