//
//  GameController.hpp
//  Gems
//
//  Created by Sunil Patel on 12/11/2015.
//
//

#ifndef GameController_hpp
#define GameController_hpp

#include "Dialogs.hpp"
#include "Level.hpp"
#include "Spell.hpp"

enum State {
	kStateMainMenu = 0,
	kStateGame,
	kStateMap,
	kStateSpellbook,
	
	kStateCount
};

class RoundDef;
class NavigationBar;

class GameController {
public:
	static void init(Scene *root);
	static GameController *get();
	
	// Game states
	void setState(State state);
	void startArena();
	void startRound(RoundDef *);
	
	// (Model) Dialogs
	void popDialog();
	void showSpellInfoDialog(Spell *);
	void showPreLevelDialog(RoundDef *round);
	void showOptionsDialog();
	
	Layer *getScreen(State state = kStateCount);
	void enableBar(bool enable);
private:
	static GameController *instance;
	Scene *root;
	NavigationBar *bar;
	State state = kStateMainMenu;
	std::vector<Dialog *> dialog_stack;
};

#endif /* GameController_hpp */
