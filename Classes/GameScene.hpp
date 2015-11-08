#ifndef GameScene_hpp
#define GameScene_hpp

#include "Gem.hpp"
#include "Grid.hpp"
#include "Spell.hpp"
#include "Characters.hpp"


class GameHUD : public cocos2d::Layer {
public:
	virtual bool init();
	void updateValues(Character *left, Character *right);
	
	CREATE_FUNC(GameHUD);
private:
	Label *left_health;
	Label *right_health;
};

class Game : public cocos2d::Layer {
public:
	static Game *get();
    Grid *grid;
    
    virtual bool init();
    void update(float dt);
	
	// Called by Spell when you cast something
	bool onCastSpell(Chain *chain);
    
    // implement the "static create()" method manually
    CREATE_FUNC(Game);
private:
	static Game *instance;
	GameHUD *hud;
	Enemy *enemy;
	Wizard *wizard;
};

#endif /* GameScene_hpp */
