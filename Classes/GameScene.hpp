#ifndef GameScene_hpp
#define GameScene_hpp

#include "Gem.hpp"
#include "Grid.hpp"
#include "Spell.hpp"
#include "Characters.hpp"


class GameHUD : public Layer {
public:
	virtual bool init();
	void updateValues(Character *left, Character *right);
	
	CREATE_FUNC(GameHUD);
private:
	Label *left_health;
	Label *right_health;
};

class Game : public Layer {
public:
	static Game *get();
    Grid *grid;
    
    bool init();
    void update(float dt);
	
	// Called by Spell when you cast something
	bool onCastSpell(Chain *chain);
	
	// Called when the Wizard's animations etc. have finished
	void onWizardTurnOver();
    
    // implement the "static create()" method manually
    CREATE_FUNC(Game);
private:	
	static Game *instance;
	void doSpell(Spell *);
	GameHUD *hud;
	Enemy *enemy;
	Wizard *wizard;
	bool checkGameOver();
	void enemyDoTurn();
};

#endif /* GameScene_hpp */
