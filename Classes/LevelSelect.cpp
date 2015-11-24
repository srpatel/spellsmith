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
#include "GameScene.hpp"

#include "ui/CocosGUI.h"

bool LevelSelect::init() {
	if ( !Layer::init() ) {
		return false;
	}
	
	Size visibleSize = Director::getInstance()->getVisibleSize();
	setContentSize(visibleSize);
	
	// Title
	auto label = Label::createWithTTF( _("level select"), Fonts::TITLE_FONT, Fonts::TITLE_SIZE);
	label->setPosition(Vec2(visibleSize.width/2,
							visibleSize.height - label->getContentSize().height));
	this->addChild(label, 1);
	
	// Add a bunch of level buttons
	{auto button = ui::Button::create("ui/button.png", "ui/buttondown.png", "ui/buttondisabled.png", ui::Widget::TextureResType::PLIST);
	button->setTitleFontName(Fonts::TEXT_FONT);
	button->setTitleText _("single level");
	
	button->setPosition(visibleSize/2);
	button->addTouchEventListener([](Ref* pSender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			Game::get()->mode = kModeLevel;
			GameController::get()->setState(kStateGame);
		}
	});
	this->addChild(button);}
	
	{auto button = ui::Button::create("ui/button.png", "ui/buttondown.png", "ui/buttondisabled.png", ui::Widget::TextureResType::PLIST);
	button->setTitleFontName(Fonts::TEXT_FONT);
	button->setTitleText _("spellbook");
	
	button->setPosition(Vec2(visibleSize.width/2, button->getContentSize().height));
	button->addTouchEventListener([](Ref* pSender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			//GameController::get()->setState(kStateGame);
		}
	});
	this->addChild(button);}
	
	{auto button = ui::Button::create("ui/button.png", "ui/buttondown.png", "ui/buttondisabled.png", ui::Widget::TextureResType::PLIST);
		button->setTitleFontName(Fonts::TEXT_FONT);
		button->setTitleText _("infinite mode");
		
		button->setPosition(Vec2(visibleSize.width/2, 2 * button->getContentSize().height));
		button->addTouchEventListener([](Ref* pSender, ui::Widget::TouchEventType type) {
			if (type == ui::Widget::TouchEventType::BEGAN) {
				Game::get()->mode = kModeInfinite;
				GameController::get()->setState(kStateGame);
			}
		});
		this->addChild(button);}
	
	return true;
}