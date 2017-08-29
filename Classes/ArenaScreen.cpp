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
	
	Size size = Director::getInstance()->getVisibleSize();
	setContentSize(size);
	
	auto button = ui::Button::create("ui/button_up.png", "ui/button_down.png", "ui/button_down.png", TEXTURE_TYPE);
	button->setTitleFontName(Fonts::TEXT_FONT);
	button->setTitleFontSize(Fonts::TEXT_SIZE);
	button->setPosition(Vec2(size.width/2, 50 + button->getContentSize().height/2));
	button->setTitleText _("ui.START_ARENA");
	button->addTouchEventListener([button](Ref* pSender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			button->setTouchEnabled(false);
			PLAY_SOUND( kSoundEffect_UISelect );
			GameController::get()->startArena();
		}
	});
	this->addChild(button);
	
	return true;
}
