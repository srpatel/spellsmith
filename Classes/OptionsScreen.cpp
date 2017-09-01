//
//  OptionsScreen.cpp
//  Gems
//
//  Created by Sunil Patel on 01/09/2017.
//
//

#include "OptionsScreen.hpp"

bool OptionsScreen::init() {
	if ( !Screen::init() ) {
		return false;
	}
	
	Size size = Director::getInstance()->getVisibleSize();
	setContentSize(size);
	
	return true;
}
