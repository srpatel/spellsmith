//
//  Spellbook.cpp
//  Gems
//
//  Created by Sunil Patel on 18/01/2017.
//
//

#include "Spellbook.hpp"

bool Spellbook::init() {
	if ( !Layer::init() ) {
		return false;
	}
	
	Size visibleSize = Director::getInstance()->getVisibleSize();
	setContentSize(visibleSize);
	
	return true;
}

void Spellbook::refreshSpells() {
	// 
}
