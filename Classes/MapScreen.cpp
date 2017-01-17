//
//  MapScreen.cpp
//  Gems
//
//  Created by Sunil Patel on 16/01/2017.
//
//

#include "MapScreen.hpp"

bool MapScreen::init() {
	if ( !Layer::init() ) {
		return false;
	}
	
	auto size = Director::getInstance()->getVisibleSize();
	
	// If you click on a level, go to that level.
	// If you click back, go to main menu.
	// If you click on spell book, go to spell book.
	
	// Back button
	auto closebutton = LoadSprite("ui/back.png");
	closebutton->setAnchorPoint(Vec2(0, 1));
	closebutton->setPosition(0, size.height);
	this->addChild(closebutton, 1);
	// on click...
	
	// Bottom bar for adventure mode
	
	return true;
}
