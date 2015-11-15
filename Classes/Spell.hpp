//
//  Spell.hpp
//  GemGame
//
//  Created by Sunil Patel on 06/11/2015.
//
//

#ifndef Spell_hpp
#define Spell_hpp

#include "Grid.hpp"
#include "Gem.hpp"

USING_NS_CC;

enum EffectType {
	Projectile = 0,
	Heal,
	Shield
};

struct BaseEffect {
	EffectType type;
};

struct EffectProjectile : public BaseEffect {
	int damage;
};

struct EffectHeal : public BaseEffect {
	int amount;
};

struct EffectShield : public BaseEffect {
	int amount;
};

class Spell {
public:
	Spell(std::string name);
	~Spell();
	static void init(int width, int height);
	static std::vector<Spell *> spells;
	std::vector<BaseEffect *> effects;
	void setup();
	Node *mininode;
	bool operator==(Chain *chain);
	std::string getName();
private:
	std::string name;
	GemType *shape;
	int width;
	int height;
	static int max_width;
	static int max_height;
	static GemType *compare_temp;
	static void rotate(GemType *grid);
	static void align(GemType *grid);
	static void flip(GemType *grid);
	static void copy(Chain *chain, GemType *grid);
};

#endif /* Spell_hpp */
