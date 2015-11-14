//
//  LevelSelect.cpp
//  Gems
//
//  Created by Sunil Patel on 13/11/2015.
//
//

#include "LevelSelect.hpp"
#include "GameController.hpp"
#include "Constants.h"
#include "Strings.hpp"

#include "ui/CocosGUI.h"

bool LevelSelect::init() {
	if ( !Layer::init() ) {
		return false;
	}
	
	Size visibleSize = Director::getInstance()->getVisibleSize();
	setContentSize(visibleSize);
	
	// Title
	auto label = Label::createWithTTF( _("level select"), Fonts::MAIN_FONT, 32);
	label->setPosition(Vec2(visibleSize.width/2,
							visibleSize.height - label->getContentSize().height));
	this->addChild(label, 1);
	
	// Add a bunch of level buttons
	{auto button = ui::Button::create("ui/button.png", "ui/button.png", "ui/button.png", ui::TextureResType::PLIST);
	button->setTitleFontName(Fonts::MAIN_FONT);
	button->setTitleText _("single level");
	
	button->setPosition(visibleSize/2);
	button->addTouchEventListener([](Ref* pSender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::BEGAN) {
			GameController::get()->setState(kStateGame);
		}
	});
	this->addChild(button);}
	
	{auto button = ui::Button::create("ui/button.png", "ui/button.png", "ui/button.png", ui::TextureResType::PLIST);
	button->setTitleFontName(Fonts::MAIN_FONT);
	button->setTitleText _("inventory");
	
	button->setPosition(Vec2(visibleSize.width/2, button->getContentSize().height));
	button->addTouchEventListener([](Ref* pSender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::BEGAN) {
			GameController::get()->setState(kStateGame);
		}
	});
	this->addChild(button);}
	
	{auto button = ui::Button::create("ui/button.png", "ui/button.png", "ui/button.png", ui::TextureResType::PLIST);
		button->setTitleFontName(Fonts::MAIN_FONT);
		button->setTitleText _("infinite mode");
		
		button->setPosition(Vec2(visibleSize.width/2, 2 * button->getContentSize().height));
		button->addTouchEventListener([](Ref* pSender, ui::Widget::TouchEventType type) {
			if (type == ui::Widget::TouchEventType::BEGAN) {
				GameController::get()->setState(kStateGame);
			}
		});
		this->addChild(button);}
	
	return true;
}