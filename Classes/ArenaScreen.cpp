//
//  ArenaScreen.cpp
//  Gems
//
//  Created by Sunil Patel on 29/08/2017.
//
//

#include "ArenaScreen.hpp"
#include "SoundManager.hpp"
#include "ImageManager.hpp"
#include "SaveData.hpp"
#include "GameController.hpp"
#include "Strings.hpp"
#include "Constants.h"

#include "ui/CocosGUI.h"

bool ArenaScreen::init() {
	if ( !Layer::init() ) {
		return false;
	}
	
	Size visibleSize = Director::getInstance()->getVisibleSize();
	setContentSize(visibleSize);
	
	return true;
}
