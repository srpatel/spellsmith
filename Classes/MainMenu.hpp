//
//  MainMenu.hpp
//  Gems
//
//  Created by Sunil Patel on 12/11/2015.
//
//

#ifndef MainMenu_hpp
#define MainMenu_hpp

#include "Screen.hpp"

class MainMenu : public Screen {
public:
	bool init();
	virtual void onSelect(){}
	CREATE_FUNC(MainMenu);
};

#endif /* MainMenu_hpp */
