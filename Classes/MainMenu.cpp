//
//  MainMenu.cpp
//  Gems
//
//  Created by Sunil Patel on 12/11/2015.
//
//

#include "MainMenu.hpp"
#include "SoundManager.hpp"
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
	
	float availableHeight = title->getPosition().y - title->getContentSize().height/2;
	int numButtons = 2;
	
	auto dy = availableHeight / (numButtons + 1);
	float y = dy;
	for (int i = numButtons - 1; i >= 0; i--, y += dy) {
		auto button = ui::Button::create("ui/button_up.png", "ui/button_down.png", "ui/button_down.png", TEXTURE_TYPE);
		button->setTitleFontName(Fonts::TEXT_FONT);
		button->setTitleFontSize(Fonts::TEXT_SIZE);
		button->setPosition(Vec2(visibleSize.width/2, y));
		
		if (i == 0) {
			button->setTitleText _("ui.ADVENTURE");
			button->addTouchEventListener([](Ref* pSender, ui::Widget::TouchEventType type) {
				if (type == ui::Widget::TouchEventType::ENDED) {
					SoundManager::get()->playEffect( kSoundEffect_Click );
					// Go to map screen
					GameController::get()->setState(kStateMap);
				}
			});
		} else if (i == 1) {
			button->setTitleText _("ui.ARENA");
			button->addTouchEventListener([](Ref* pSender, ui::Widget::TouchEventType type) {
				if (type == ui::Widget::TouchEventType::ENDED) {
					// For now, from scratch always!
					SoundManager::get()->playEffect( kSoundEffect_Click );
					GameController::get()->startGame(nullptr);
				}
			});
		}
		this->addChild(button);
	}
	
	return true;
}
