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
	
	auto sprite = LoadSprite("ui/smokey.png");
	sprite->setAnchorPoint(Vec2(0.5, 0.5));
	sprite->setPosition(visibleSize/2);
	this->addChild(sprite);
	
	// Title
	auto title = LoadSprite("main/logo.png");
	title->setAnchorPoint(Vec2(0.5, 0.5));
	title->setPosition(visibleSize.width/2, visibleSize.height * 0.75);
	this->addChild(title);
	
	// Play button
	auto button = ui::Button::create("ui/button_up.png", "ui/button_down.png", "ui/button_down.png", TEXTURE_TYPE);
	button->setTitleFontName(Fonts::TEXT_FONT);
	button->setTitleText _("New Game");
	
	button->setPosition(visibleSize/2);
	button->addTouchEventListener([](Ref* pSender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			// For now, from scratch always!
			GameController::get()->startGame(nullptr);
		}
	});
	this->addChild(button);
	
	return true;
}