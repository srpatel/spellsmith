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
#include "Projectiles.hpp"
#include "ColumnScreen.hpp"
#include "Online.hpp"

typedef std::function<void()> PendingAction;

enum GameState {
	kStatePlayerTurn,
	kStatePlayerSpells,
	kStateEnemySpells
};

class RoundDef;

class Game : public ColumnScreen {
	friend class DoSpell;
	friend class Tutorial;
public:
	static Game *get();
    Grid *grid;
	
	virtual void onSelect();
	virtual void onDeselect();
	
	ArenaScore getArenaScore();
	
	// Restart the current game!
	void startArena(std::string state);
	void startRound(RoundDef *round);
	void restartRound();
	
	Wizard *getWizard();
	void updateInventory();
	void spellPicked();
	
    bool init();
	
	// Called by Spell when you cast something
	bool onCastSpell(Chain *chain);
	
	// Called when the Wizard's animations etc. have finished
	void onWizardTurnOver();
	
	layout_t getLayout();
	
	GameScenery *scenery;
	
    Wizard *wizard;
	void updateHealthBars();
	
	Sprite *mute_button;
	
    // implement the "static create()" method manually
    CREATE_FUNC(Game);
private:
	static Game *instance;
	
	int stage; // the current round in infinite mode, otherwise irrelevant.
	
	void setup();
	
	void gotoNextEnemy();
	void showRound(RoundDef *, int wave);
	void saveArenaState();
	
	int getNextAliveEnemy(int start, bool *allDead);
	
	GameHUD *hud;
	std::vector<Enemy *> enemies;
	bool setSelected(int);
	int currentEnemy;
	std::vector<Spell *> spellpool;
	Node *wizard_hp_bar;
	Label *wizard_hp_text;
	Label *currentRound;
	Label *currentWaveHeading;
	Label *currentWave;
	Node *level_counter;
	Node *post_level_dialog;
	Node *game_over_dialog;
	Layer *inventoryHolder;
	std::vector<EventListener *> inventoryListeners;
	bool checkGameOver();
	void enemyDoTurn();
	GameState state;
	void attemptSetState(GameState);
	int wave;
	int numMoves = 0;
	RoundDef *round;
	
	// Helper
	void onDamageTarget(Character *target, bool withDelay); // Called to show the target taking damage (recoil/death animation, flash red)
	
	// Spell effects
	void wizardBashAnimationByQueue();
	void makeProjectile(Character *source, Character *target, int damage, ProjectileType type, std::function<void(void)> onhit = {} /*default to no callable*/);
	void makeMeteor(float xpos, float ypos, float delay);
	void makeCracks(Character *target);
	void makeLightning(Character *target, int amt);
	
	int numCurrentActions;
	
	// An action has finished (must have called actionQueued previously)
	void actionDone();
	// Start an action ... must call actionDone when finished
	void actionQueued();
	// Run an action when all current actions are done.
	void runPendingAction(PendingAction);
	
	std::vector<PendingAction> pendingActions;
};

#endif /* GameScene_hpp */
