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

class Game;

typedef std::function<int(Game *)> AmountGenerator;

enum EffectType {
	Invalid,
	Projectile,
	ChangeHealth,
	Shield
};

enum EffectTarget {
	None,
	Target,
	Other,
	Self,
	All
};

struct SpellEffect {
	EffectType type;
	EffectTarget target;
	int key;
	int wait_key;
	AmountGenerator amountGenerator;
};

class Spell {
	friend class SpellManager;
public:
	Spell(std::string name);
	~Spell();
	std::vector<SpellEffect *> effects;
	void setup();
	Layer *mininode;
	bool operator==(Chain *chain);
	std::string getName();
	std::string getDescription();
	int tier;
private:
	std::string name;
	std::string description;
	GemType *shape;
	int width;
	int height;
	static void rotate(GemType *grid);
	static void align(GemType *grid);
	static void flip(GemType *grid);
	static void copy(Chain *chain, GemType *grid);
};

class SpellManager {
public:
	static SpellManager *get();
	void init();
	std::vector<Spell *> spells;
private:
	static SpellManager *instance;
};

#endif /* Spell_hpp */
