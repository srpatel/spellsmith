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

#include "ui/CocosGUI.h"

bool GameOverPopup::init() {
	if ( !Layer::init() ) {
		return false;
	}
	
	auto size = Size(200, 150);
	auto popup = Popup::create(size.width, size.height);
	popup->setPosition(size/-2);
	this->addChild(popup);
	setContentSize(size);
	
	// "Pick a spell" title
	auto label = Label::createWithTTF( "game over", Fonts::TITLE_FONT, Fonts::TEXT_SIZE);
	label->setColor(Color3B::WHITE);
	label->setPosition(Vec2(0, 50));
	addChild(label, 1);
	
	// "return to menu" button
	auto button = ui::Button::create("ui/button_up.png", "ui/button_down.png", "ui/button_down.png", TEXTURE_TYPE);
	button->setTitleFontName(Fonts::TEXT_FONT);
	button->setTitleText("Back to map");
	button->setPosition(Vec2(0, -50));
	button->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			// For now, from scratch always!
			removeFromParent();
			GameController::get()->setState(kStateMap);
		}
	});
	addChild(button);

	return true;
}
