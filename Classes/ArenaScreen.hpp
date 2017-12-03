//
//  ArenaScreen.hpp
//  Gems
//
//  Created by Sunil Patel on 29/08/2017.
//
//

#ifndef ArenaScreen_hpp
#define ArenaScreen_hpp

#include "ColumnScreen.hpp"
#include "ui/CocosGUI.h"

class ArenaScreen : public ColumnScreen {
public:
	bool init();
	virtual void onSelect();
	CREATE_FUNC(ArenaScreen);
private:
	ui::Button *button;
};

#endif /* ArenaScreen_hpp */
