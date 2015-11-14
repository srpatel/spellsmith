//
//  GameController.hpp
//  Gems
//
//  Created by Sunil Patel on 12/11/2015.
//
//

#ifndef GameController_hpp
#define GameController_hpp

enum State {
	kStateMainMenu = 0,
	kStateLevelSelect,
	kStateGame,
	
	
	kStateCount
};

class GameController {
public:
	static void init(Scene *root);
	static GameController *get();
	
	void setState(State state);
private:
	static GameController *instance;
	Scene *root;
	State state = kStateMainMenu;
};

#endif /* GameController_hpp */
