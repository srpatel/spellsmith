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

typedef std::function<void()> PendingAction;

enum GameState {
	kStatePlayerTurn,
	kStatePlayerSpells,
	kStateEnemySpells
};

struct layout_t {
	float ui_scale;
	float column_height;
	float column_width;
	float bar_top_height;
	float bar_bottom_height;
	float scenery_height;
	Vec2 melee_spot; //where enemies stand to hit the wizard
};

class RoundDef;

class Game : public Layer {
	friend class DoSpell;
public:
	static Game *get();
    Grid *grid;
	
	// Restart the current game!
	void startArena();
	void startRound(RoundDef *round);
	
	Wizard *getWizard();
	void updateInventory();
	void spellPicked();
	
    bool init();
    void update(float dt);
	
	// Called by Spell when you cast something
	bool onCastSpell(Chain *chain);
	
	// Called when the Wizard's animations etc. have finished
	void onWizardTurnOver();
	
	layout_t getLayout();
	GameScenery *scenery;
    Wizard *wizard;
	void updateHealthBars();
	
    // implement the "static create()" method manually
    CREATE_FUNC(Game);
private:
	static Game *instance;
	
	int stage; // the current round in infinite mode, otherwise irrelevant.
	
	void setup();
	
	void gotoNextEnemy();
	void showRound(RoundDef *);
	
	int getNextAliveEnemy(int start, bool *allDead);
	
	GameHUD *hud;
	std::vector<Enemy *> enemies;
	bool setSelected(int);
	int currentEnemy;
	std::vector<Spell *> spellpool;
	Node *wizard_hp_bar;
	Label *wizard_hp_text;
	Label *currentRound;
	Node *level_counter;
	Layer *inventoryHolder;
	std::vector<EventListener *> inventoryListeners;
	bool checkGameOver();
	void enemyDoTurn();
	GameState state;
	void attemptSetState(GameState);
	RoundDef *round;
	int numMoves = 0;
	
	// Helper
	void onDamageTarget(Character *target, bool withDelay); // Called to show the target taking damage (recoil/death animation, flash red)
	
	// Spell effects
	void makeProjectile(Character *source, Character *target, int damage, Color3B type,std::function<void(void)> onhit = {} /*default to no callable*/);
	void makeMeteor(float xpos, float ypos, float delay);
	void makeCracks(Character *target);
	void makeLightning(Character *target);
	
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
