//
//  NavigationBar.cpp
//  Gems
//
//  Created by Sunil Patel on 12/11/2015.
//
//

#include "NavigationBar.hpp"
#include "SoundManager.hpp"
#include "ImageManager.hpp"
#include "GameController.hpp"
#include "Strings.hpp"
#include "Constants.h"
#include "SaveData.hpp"

#include "ui/CocosGUI.h"


struct ButtonDef {
	const char *label;
	const char *imagePath;
	State state;
};

void NavigationBar::resetButtons() {
	// Buttons
	std::vector<ButtonDef> buttons;
	buttonHolder->removeAllChildren();
	
	auto spells = SaveData::getSpells();
	if (! spells.empty()) {
		buttons.push_back({"SPELLBOOK", "icons/spellbook.png", kStateSpellbook}); // Set state to spellbook
	}
	if (SaveData::isLevelComplete("3")) {
		buttons.push_back({"ARENA", "icons/map.png", kStateArena}); // Set state to arena
	}

	int num = buttons.size();
	float widthPerButton = getContentSize().width / num;
	float currentX = widthPerButton / 2.0f;
	for (ButtonDef b : buttons) {
		auto n = LoadSprite(b.imagePath);
		n->setAnchorPoint(Vec2(0.5, 0.5));
		n->setPosition(currentX, getContentSize().height/2.0f);
		buttonHolder->addChild(n);
		auto t = Label::createWithTTF(b.label, Fonts::TEXT_FONT, Fonts::TEXT_SIZE);
		t->setTextColor(Color4B::BLACK);
		t->setPosition(currentX, 10);
		buttonHolder->addChild(t);
		currentX += widthPerButton;
		// Add onclick...
		auto onclick = EventListenerTouchOneByOne::create();
		onclick->setSwallowTouches(true);
		// trigger when you push down
		onclick->onTouchBegan = [this, n, b](Touch* touch, Event* event) -> bool {
			Vec2 p = touch->getLocation();
			Rect rect = n->getBoundingBox();
			rect.origin += getPosition();
			if(enabled && rect.containsPoint(p)) {
				PLAY_SOUND( kSoundEffect_UISelect );
				GameController::get()->setState(b.state);
				return true;
			}
			
			return false; // if you are consuming it
		};
		// get removed when children are removed
		_eventDispatcher->addEventListenerWithSceneGraphPriority(onclick, n);
	}
}


bool NavigationBar::init() {
	if ( !Layer::init() ) {
		return false;
	}
	
	Size visibleSize = Director::getInstance()->getVisibleSize();
	setContentSize(Size(visibleSize.width, NavigationBar::HEIGHT));
	
	auto sprite = LoadSprite("ui/nav_bar.png");
	auto ratio = 55.0f / sprite->getContentSize().height;
	sprite->setAnchorPoint(Vec2(0.5, 1));
	sprite->setScale(3, -ratio);
	sprite->setPosition(Vec2(visibleSize.width/2, 0));
	//	sprite->setScale(ui_scale);
	addChild(sprite);
	
	buttonHolder = Layer::create();
	addChild(buttonHolder);
	
	resetButtons();

	return true;
}
