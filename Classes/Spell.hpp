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

class Spell {
public:
	Spell();
	~Spell();
	static void init(int width, int height);
	static std::vector<Spell *> spells;
	void setup();
	Node *mininode;
	bool operator==(Chain *chain);
private:
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
