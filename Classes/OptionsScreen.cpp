//
//  OptionsScreen.cpp
//  Gems
//
//  Created by Sunil Patel on 01/09/2017.
//
//

#include "OptionsScreen.hpp"
#include "Constants.h"

bool OptionsScreen::init() {
	if ( !Screen::init() ) {
		return false;
	}
	
	Size size = Director::getInstance()->getVisibleSize();
	setContentSize(size);
	
	auto label = Label::createWithTTF(
		"Here you can:\n"
		"- Mute/unmute\n"
		"- Change language\n"
		"- Clear save\n"
		"- See credits / links to websites",
		Fonts::TEXT_FONT, Fonts::TEXT_SIZE);
	label->setDimensions(size.width - 20, 0);
	label->setAlignment(TextHAlignment::CENTER);
	// position the label on the center of the screen
	label->setPosition(size/2);
	addChild(label);
	
	return true;
}
