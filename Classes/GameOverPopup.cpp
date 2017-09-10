//
//  GameOverPopup.cpp
//  Gems
//
//  Created by Sunil Patel on 13/08/2016.
//
//

#include "GameOverPopup.hpp"
#include "GameController.hpp"
#include "Constants.h"
#include "Popup.hpp"
#include "GameScene.hpp"
#include "SaveData.hpp"
#include "Strings.hpp"

#include "ui/CocosGUI.h"

bool GameOverPopup::init(bool isMain) {
	if ( !Layer::init() ) {
		return false;
	}
	
	// "Pick a spell" title
	/*auto label = Label::createWithTTF( _("level_end.GAME_OVER"), Fonts::TITLE_FONT, Fonts::TEXT_SIZE);
	label->setColor(Color3B::WHITE);
	addChild(label, 1);*/
	
	// Retry button
	ui::Button *button1 = ui::Button::create("ui/button_up.png", "ui/button_down.png", "ui/button_down.png", TEXTURE_TYPE);
	button1->setTitleFontName(Fonts::TEXT_FONT);
	button1->setTitleFontSize(Fonts::TEXT_SIZE);
	button1->setTitleText _("ui.RETRY_LEVEL");
	button1->addTouchEventListener([this, button1, isMain](Ref* pSender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			// For now, from scratch always!
			button1->setTouchEnabled(false);
			if (isMain) {
				GameController::get()->setState(kStateGame);
				Game::get()->restartRound();
			} else {
				GameController::get()->startArena(SaveData::getArenaState());
			}
		}
	});
	addChild(button1, 1);
	
	// "return to menu" button
	auto button2 = ui::Button::create("ui/button_up.png", "ui/button_down.png", "ui/button_down.png", TEXTURE_TYPE);
	button2->setTitleFontName(Fonts::TEXT_FONT);
	button2->setTitleFontSize(Fonts::TEXT_SIZE);
	button2->setTitleText _("ui.RETURN_TO_MAP");
	button2->addTouchEventListener([this, button2](Ref* pSender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			// For now, from scratch always!
			button2->setTouchEnabled(false);
			GameController::get()->setState(kStateMap);
		}
	});
	addChild(button2, 1);
	
	
	Size size {
		button2->getContentSize().width + 30,
		//10 +
		//label->getContentSize().height +
		15 +
		(button1 == nullptr ?
			0 :
			(button1->getContentSize().height + 10)) +
		button2->getContentSize().height +
		15
	};
	
	auto popup = Popup::create(size.width, size.height);
	popup->setPosition(size/-2);
	this->addChild(popup);
	setContentSize(size);
	
	//label->setPosition({0, size.height/2 - 10 - label->getContentSize().height / 2});
	if (button1 != nullptr) {
		button1->setPosition({0, size.height/2 - 15 - button1->getContentSize().height/2});
	}
	button2->setPosition({0, -size.height/2 + 15 + button2->getContentSize().height/2});

	return true;
}
