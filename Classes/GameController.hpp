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
#include "Screen.hpp"

enum State {
	kStateMainMenu = 0,
	kStateGame,
	kStateMap,
	kStateSpellbook,
	kStateArena,
	
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
	void pushDialog(Dialog *dialog);
	void popDialog();
	void showSpellInfoDialog(Spell *);
	void showPreLevelDialog(RoundDef *round);
	void showOptionsDialog();
	
	Screen *getScreen(State state = kStateCount);
	void showButton(bool show);
private:
	static GameController *instance;
	Scene *root;
	NavigationBar *bar;
	Node *button;
	Node *fader;
	State state = kStateMainMenu;
	std::vector<Dialog *> dialog_stack;
};

#endif /* GameController_hpp */
