//
//  MapScreen.cpp
//  Gems
//
//  Created by Sunil Patel on 16/01/2017.
//
//

#include "MapScreen.hpp"
#include "ui/CocosGUI.h"
#include "Strings.hpp"
#include "Constants.h"
#include "GameController.hpp"

bool MapScreen::init() {
	if ( !Layer::init() ) {
		return false;
	}
	
	auto size = Director::getInstance()->getVisibleSize();
	
	// If you click on a level, go to that level.
	// If you click back, go to main menu.
	// If you click on spell book, go to spell book.
	auto button = ui::Button::create("ui/button_up.png", "ui/button_down.png", "ui/button_down.png", TEXTURE_TYPE);
	button->setTitleFontName(Fonts::TEXT_FONT);
	button->setTitleFontSize(Fonts::TEXT_SIZE);
	button->setPosition(Vec2(size.width/2, size.height/2));
	button->setTitleText _("ui.ARENA");
	button->addTouchEventListener([](Ref* pSender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			// For now, from scratch always!
			GameController::get()->startGame(nullptr);
		}
	});
	this->addChild(button);
	
	// Bottom bar for adventure mode
	
	return true;
}
