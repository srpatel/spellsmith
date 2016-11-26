//
//  MainMenu.cpp
//  Gems
//
//  Created by Sunil Patel on 12/11/2015.
//
//

#include "MainMenu.hpp"
#include "ImageManager.hpp"
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
	
	auto sprite = LoadLargeSprite("smokey.png");
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

	/*auto boy = spine::SkeletonAnimation::createWithFile("spine/spineboy.json", "spine/spineboy.atlas", 0.6f);
	boy->setScale(0.4);
	boy->setPosition(visibleSize.width/2, 0);
	// Set transitions from walk -> jump, jump -> run and run -> walk
	boy->setMix("walk", "jump", 0.2f);
	boy->setMix("jump", "run", 0.2f);
	
	boy->setMix("run", "walk", 0.2f);
	boy->setAnimation(0, "walk", true);
	boy->addAnimation(0, "jump", false, 3);
	boy->addAnimation(0, "run", true);
	
	for (int i = 0; i < 100; i++) {
		boy->addAnimation(0, "walk", true, 3);
		boy->addAnimation(0, "jump", false, 3);
		boy->addAnimation(0, "run", true);
	}
	
	addChild(boy);*/
	return true;
}
