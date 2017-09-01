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
#include "Buff.hpp"

enum CharacterType {
	Humanoid = 0,
	Animal
};

class Character {
public:
	int max_health;
	int health;
	int ui_health;
	CharacterType type;
	bool is_skeleton = false;
	float projectile_height;
	Vec2 head_offset;
	Node *sprite;
	std::vector<Buff *> buffs;
	Buff *getBuffByType(BuffType);
	void addBuff(Buff *, bool apply = true);
	void removeBuff(Buff *);
	void clearBuffs();
	void updateBuffs();
	void tickBuffs();
	inline bool dead() { return health <= 0; }
	inline bool ui_dead() { return ui_health <= 0; }
	Layer *buffHolder;
	void flash(Color3B c);
	void heal(int amt);
	float die();
	void damageEffect(int damage);
	virtual std::map<std::string, Vec2> getOffsets() = 0;
	virtual ~Character();
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
