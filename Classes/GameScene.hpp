#ifndef GameScene_hpp
#define GameScene_hpp

#include "Gem.hpp"
#include "Grid.hpp"
#include "Spell.hpp"
#include "Dialogs.hpp"
#include "Characters.hpp"
#include "GameHUD.hpp"
#include "GameScenery.hpp"
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

class Game : public Layer {
public:
	static Game *get();
    Grid *grid;
	
	// Restart the current game!
	void startGame(SaveGame *save);
	
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
	int stage; // the current monster you are facing, or the level you are on in infinite mode.
	
	void gotoNextEnemy();
	void showRound(Round *);
	
	GameHUD *hud;
	GameScenery *scenery;
	std::vector<Enemy *> enemies;
	int currentEnemy;
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
