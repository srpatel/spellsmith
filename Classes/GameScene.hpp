#ifndef GameScene_hpp
#define GameScene_hpp

#include "Gem.hpp"
#include "Grid.hpp"
#include "Spell.hpp"

class Game : public cocos2d::Layer
{
public:
	static Game *get();
    Grid *grid;
    
    virtual bool init();
    void update(float dt);
	
	bool onCastSpell(Chain *chain);
    
    // implement the "static create()" method manually
    CREATE_FUNC(Game);
private:
	static Game *instance;
};

struct PlayerState {
	static PlayerState *get();
	// Stuff which must get saved between plays...
	std::vector<Spell *> inventory;
};

#endif /* GameScene_hpp */
