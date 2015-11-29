//
//  LevelSelect.cpp
//  Gems
//
//  Created by Sunil Patel on 13/11/2015.
//
//
#include "Level.hpp"
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
	button->setTitleText("1");
	
	button->setPosition(visibleSize/2);
	button->addTouchEventListener([](Ref* pSender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			if (LevelManager::get()->forest_levels.size() >= 1) {
				GameController::get()->startGame(LevelManager::get()->forest_levels[0]);
			}
		}
	});
	this->addChild(button);}
	{auto button = ui::Button::create("ui/button.png", "ui/buttondown.png", "ui/buttondisabled.png", ui::Widget::TextureResType::PLIST);
		button->setTitleFontName(Fonts::TEXT_FONT);
		button->setTitleText("2");
		
		button->setPosition(Vec2(visibleSize/2) + Vec2(0, button->getBoundingBox().size.height));
		button->addTouchEventListener([](Ref* pSender, ui::Widget::TouchEventType type) {
			if (type == ui::Widget::TouchEventType::ENDED) {
					if (LevelManager::get()->forest_levels.size() >= 2) {
				GameController::get()->startGame(LevelManager::get()->forest_levels[1]);
					}
			}
		});
		this->addChild(button);}
	{auto button = ui::Button::create("ui/button.png", "ui/buttondown.png", "ui/buttondisabled.png", ui::Widget::TextureResType::PLIST);
		button->setTitleFontName(Fonts::TEXT_FONT);
		button->setTitleText("3");
		
		button->setPosition(Vec2(visibleSize/2) + Vec2(0, button->getBoundingBox().size.height) * 2);
		button->addTouchEventListener([](Ref* pSender, ui::Widget::TouchEventType type) {
			if (type == ui::Widget::TouchEventType::ENDED) {
				if (LevelManager::get()->forest_levels.size() >= 3) {
				GameController::get()->startGame(LevelManager::get()->forest_levels[2]);
				}
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
				GameController::get()->startGame(nullptr);
			}
		});
		this->addChild(button);}
	
	return true;
}