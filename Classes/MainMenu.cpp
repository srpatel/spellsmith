//
//  MainMenu.cpp
//  Gems
//
//  Created by Sunil Patel on 12/11/2015.
//
//

#include "MainMenu.hpp"
#include "GameController.hpp"
#include "Strings.hpp"
#include "Constants.h"

#include "ui/CocosGUI.h"

bool MainMenu::init() {
	if ( !Layer::init() ) {
		return false;
	}
	
	Size visibleSize = Director::getInstance()->getVisibleSize();
	setContentSize(visibleSize);
	
	// Title
	auto label = Label::createWithTTF( _("spellsmith"), Fonts::TITLE_FONT, Fonts::TITLE_SIZE);
	label->setPosition(Vec2(visibleSize.width/2,
							visibleSize.height - label->getContentSize().height));
	this->addChild(label, 1);
	
	// Play button
	auto button = ui::Button::create("ui/button.png", "ui/buttondown.png", "ui/buttondisabled.png", TEXTURE_TYPE);
	button->setTitleFontName(Fonts::TEXT_FONT);
	button->setTitleText _("Play");
	
	button->setPosition(visibleSize/2);
	button->addTouchEventListener([](Ref* pSender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			GameController::get()->setState(kStateLevelSelect);
		}
	});
	this->addChild(button);
	
	return true;
}