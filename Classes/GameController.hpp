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
	
	kStateCount
};

class GameController {
public:
	static void init(Scene *root);
	static GameController *get();
	
	// Game states
	void setState(State state);
	void startGame(SaveGame *save);
	
	// (Model) Dialogs
	void popDialog();
	void showLevelEndDialog(bool victory);
	void showSpellInfoDialog(Spell *);
private:
	static GameController *instance;
	Scene *root;
	State state = kStateMainMenu;
	std::vector<Dialog *> dialog_stack;
};

#endif /* GameController_hpp */
