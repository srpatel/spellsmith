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

#include "ui/CocosGUI.h"


struct ButtonDef {
	const char *label;
	const char *imagePath;
	State state;
};


bool NavigationBar::init() {
	if ( !Layer::init() ) {
		return false;
	}
	
	Size visibleSize = Director::getInstance()->getVisibleSize();
	setContentSize(Size(visibleSize.width, NavigationBar::HEIGHT));
	
	// Background
	auto grad = LayerColor::create();
	grad->initWithColor(Color4B::WHITE);
	grad->setContentSize(getContentSize());
	addChild(grad);
	
	// Buttons
	std::vector<ButtonDef> buttons;
	buttons.push_back({"MAP", "icons/map.png", kStateMap}); // Set state to map
	buttons.push_back({"SPELLBOOK", "icons/spellbook.png", kStateSpellbook}); // Set state to spellbook

	int num = buttons.size();
	float widthPerButton = getContentSize().width / num;
	float currentX = widthPerButton / 2.0f;
	for (ButtonDef b : buttons) {
		auto n = LoadSprite(b.imagePath);
		n->setAnchorPoint(Vec2(0.5, 0.5));
		n->setPosition(currentX, getContentSize().height/2.0f);
		addChild(n);
		auto t = Label::createWithTTF(b.label, Fonts::TEXT_FONT, Fonts::TEXT_SIZE);
		t->setTextColor(Color4B::BLACK);
		t->setPosition(currentX, 10);
		addChild(t);
		currentX += widthPerButton;
		// Add onclick...
		auto onclick = EventListenerTouchOneByOne::create();
		onclick->setSwallowTouches(true);
		// trigger when you push down
		onclick->onTouchBegan = [this, n, b](Touch* touch, Event* event) -> bool {
			Vec2 p = touch->getLocation();
			Rect rect = n->getBoundingBox();
			if(rect.containsPoint(p)) {
				PLAY_SOUND( kSoundEffect_UISelect );
				GameController::get()->setState(b.state);
				return true;
			}
			
			return false; // if you are consuming it
		};
		_eventDispatcher->addEventListenerWithSceneGraphPriority(onclick, n);
	}

	return true;
}
