//
//  ArenaScreen.hpp
//  Gems
//
//  Created by Sunil Patel on 29/08/2017.
//
//

#ifndef ArenaScreen_hpp
#define ArenaScreen_hpp

#include "Screen.hpp"

class ArenaScreen : public Screen {
public:
	bool init();
	virtual void onSelect(){}
	CREATE_FUNC(ArenaScreen);
};

#endif /* ArenaScreen_hpp */
