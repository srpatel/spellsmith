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
#include "NavigationBar.hpp"
#include "GameController.hpp"
#include "Strings.hpp"
#include "Constants.h"

#include "ui/CocosGUI.h"

void ArenaScreen::onSelect() {
	GameController::get()->startArena(SaveData::getArenaState());
	/*
	button->setTouchEnabled(true);
	auto state = SaveData::getArenaState();
	if (state.empty()) {
		// button says start again?
		button->setTitleText _("ui.START_ARENA");
	} else {
		button->setTitleText _("ui.CONTINUE_ARENA");
	}
	if (SaveData::getArenaScore() <= 0) {
		std::string message = "Welcome to the arena!\n\n"
			"The arena is an infinite game mode where you face many different challengers. Get as far as you can before you get knocked out!";
		auto d = OneShotDialog::create(message);
		GameController::get()->pushDialog(d);
	}*/
}

bool ArenaScreen::init() {
	if ( !ColumnScreen::init() ) {
		return false;
	}
	
	setBackground("bg_arena.png");
	
	auto grad = LayerColor::create();
	grad->initWithColor(Color4B(95, 91, 85, 255));
	grad->setPosition(Vec2(6, 340) * layout.ui_scale);
	grad->setContentSize(Size(27, 30) * layout.ui_scale);
	this->addChild(grad, 6);
	
	Size size = Director::getInstance()->getVisibleSize();
	setContentSize(size);
	
	/*auto label = Label::createWithTTF(
		"Here you can:\n"
		"- See YOUR previous runs (e.g. score, 32 - killed by goblin)\n"
		"- See wordwide top scores\n"
		"- Continue your current run or start a new one",
		Fonts::TEXT_FONT, Fonts::TEXT_SIZE);
	label->setDimensions(size.width - 20, 0);
	label->setAlignment(TextHAlignment::CENTER);
	// position the label on the center of the screen
	label->setPosition(size/2);
	addChild(label);*/
	
	button = ui::Button::create("ui/button_up.png", "ui/button_down.png", "ui/button_down.png", TEXTURE_TYPE);
	button->setTitleFontName(Fonts::TEXT_FONT);
	button->setTitleFontSize(Fonts::TEXT_SIZE);
	button->setPosition(Vec2(size.width/2, NavigationBar::HEIGHT + 10 + button->getContentSize().height/2));
	button->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			button->setTouchEnabled(false);
			PLAY_SOUND( kSoundEffect_UISelect );
			GameController::get()->startArena(SaveData::getArenaState());
		}
	});
	
	this->addChild(button);
	
	return true;
}
