#ifndef GameScene_hpp
#define GameScene_hpp

#include "Gem.hpp"
#include "Grid.hpp"
#include "Spell.hpp"
#include "Dialogs.hpp"
#include "Characters.hpp"
#include "Level.hpp"

enum GameMode {
	kModeInfinite,
	kModeLevel,
	kModeMultiplayer
};

enum GameState {
	kStatePlayerTurn,
	kStatePlayerSpells,
	kStateEnemySpells
};

struct GameAnimation {
	Sprite *target;
	FiniteTimeAction *action;
};

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
	
	// Restart the current game!
	void resetToStartOfLevel();
	void startLevel(Level *);
	
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
	
	// Actually create the projectiles and stuff
	void doSpell(Spell *);
	
	GameMode mode = kModeInfinite;
	Level* level = nullptr;
	int stage; // the current monster you are facing, or the level you are on in infinite mode.
	
	bool gotoNextEnemy();
	void prepareStartRound();
	
	GameHUD *hud;
	Enemy *enemy;
	Wizard *wizard;
	bool checkGameOver();
	void enemyDoTurn();
	GameState state;
	void attemptSetState(GameState);
	
	void addBuff(Character *target, Buff *buff);
	void removeBuff(Character *target, Buff *buff);
	
	void makeProjectile(Character *source, Character *target, int damage, Color3B type);
	void runAnimation(GameAnimation *ga);
	std::vector<GameAnimation *> animation_queue;
	GameAnimation *currentAnimation = nullptr;
};

#endif /* GameScene_hpp */
