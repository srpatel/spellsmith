//
//  Tutorial.cpp
//  Gems
//
//  Created by Sunil Patel on 27/08/2017.
//
//

#include "Tutorial.hpp"
#include "GameScene.hpp"
#include "Popup.hpp"
#include "Constants.h"

#define kPosGrid 1
#define kPosScenery 2

int currentLevel = -1;
Layer *currentPopup = nullptr;

Layer *makeTextBox(EventDispatcher *ed, std::string text, int location, bool withTtc, float delay) {
	auto game = Game::get();
	if (currentPopup != nullptr) {
		currentPopup->removeFromParent();
	}

	auto yBuffer = 50;
	auto size = Size(200, 0);
	
	// Add wrapping text in the middle...
	auto label = Label::createWithTTF(text, Fonts::TEXT_FONT, Fonts::TEXT_SIZE);
	label->setDimensions(size.width - 20, 0);
	label->setAlignment(TextHAlignment::CENTER);
	size.height = label->getContentSize().height + yBuffer;
	// position the label on the center of the screen
	label->setPosition(size/2 + Size(0, withTtc ? 15 : 0));
	
	auto popup = Popup::create(size.width, size.height);
	currentPopup = popup;
	popup->setCascadeOpacityEnabled(true);
	popup->addChild(label);
	
	if (withTtc) {
		// tap to continue label
		auto ttc = Label::createWithTTF("[Tap to continue]", Fonts::TEXT_FONT, Fonts::SMALL_SIZE);
		ttc->setDimensions(size.width - 20, 0);
		ttc->setAlignment(TextHAlignment::CENTER);
		// position the label on the center of the screen
		ttc->setPosition(Vec2(size.width/2, 20));
		ttc->setOpacity(0);
		auto fadeIn = EaseIn::create(FadeIn::create(1), 0.5f);
		auto fadeOut = EaseOut::create(FadeOut::create(1), 0.5f);
		auto sequence = RepeatForever::create(Sequence::create(fadeIn, fadeOut, DelayTime::create(0.5f), nullptr));
		ttc->runAction(sequence);
		popup->addChild(ttc);
	}
	
	Vec2 position;
	
	if (location == kPosGrid) {
		auto gridCentre = game->grid->getPosition();
		position = gridCentre - size/2;
	} else if (location == kPosScenery) {
		auto sceneCentre = game->scenery->getPosition();
		auto sceneSize = game->scenery->getContentSize();
		position = sceneCentre + sceneSize/2 - size/2;
	}
	popup->setPosition(position);
	
	popup->setOpacity(0);
	auto fadeIn = Sequence::create(DelayTime::create(delay), FadeIn::create(0.5), nullptr);
	popup->runAction(fadeIn);
	
	// On tap, fade out and advance tutorial...
	if (withTtc) {
		auto touchListener = cocos2d::EventListenerTouchOneByOne::create();
		touchListener->setSwallowTouches(true);
		touchListener->onTouchBegan = [game, popup, size, position](Touch *touch, Event *event) {
			auto bounds = Rect(
				position,
				size
			);
			if (bounds.containsPoint(touch->getLocation())) {
				event->stopPropagation();
				Tutorial::activate(currentLevel + 1);
				return true;
			}
			return false;
		};
		ed->addEventListenerWithSceneGraphPriority(touchListener, popup);
	}
	
	return popup;
}

void Tutorial::cancelAll() {
	currentLevel = -1;
	if (currentPopup != nullptr) {
		currentPopup->removeFromParent();
		currentPopup = nullptr;
	}
}

void Tutorial::activate(int number) {
	auto game = Game::get();
	// Starting numbers: 1
	bool start = number == 1;
	bool prec = number == currentLevel + 1;
	
	// Don't progress to the next tutorial unless you're starting a chain
	// or are moving to the next tutorial.
	if (! start && ! prec)
		return;

	if (currentPopup != nullptr) {
		currentPopup->runAction(Sequence::create(FadeOut::create(1), RemoveSelf::create(), nullptr));
		currentPopup = nullptr;
	}
	currentLevel = number;
	if (number == 1) {
		// Start down this path!
		// Grid fakeout -- pre-determine grid.
		game->grid->preset(1);
		
		// Cannot use grid
		game->grid->setActive(false);
		
		// Make text box over grid...
		auto popup = makeTextBox(game->_eventDispatcher,
			"Oh, heavens! There is a goblin in the path. "
			"We'd better take care of it...",
			kPosGrid, true, 0);
		game->addChild(popup);
	} else if (number == 2) {
		game->grid->setActive(true);
		game->grid->flashPreset(1);
		// Fade out everything except the path we want to suggest
		
		auto popup = makeTextBox(game->_eventDispatcher,
			"Draw chains of the same colour to deal damage.",
			kPosScenery, false, 0.5);
		game->addChild(popup);
	} else if (number == 3) {
		// Fade everything in...
		game->grid->flashPreset(0);
	}
}
