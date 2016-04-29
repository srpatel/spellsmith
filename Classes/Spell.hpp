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

//typedef std::function<int(Game *)> AmountGenerator;

class Spell {
	friend class SpellManager;
public:
	Spell(const char *name);
	~Spell();
	void setup();
	Layer *mininode;
	bool operator==(Chain *chain);
	std::string getName();
	inline std::string getRawName() { return name; }
	std::string getDescription();
	int tier;
	//AmountGenerator amountGenerator;
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
