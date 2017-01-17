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
	char *label;
	char *imagePath;
};


bool NavigationBar::init() {
	if ( !Layer::init() ) {
		return false;
	}
	
	Size visibleSize = Director::getInstance()->getVisibleSize();
	setContentSize(visibleSize.width, 50);
	
	// Background
	auto grad = LayerColor::create();
	grad->initWithColor(Color4B(Colours::BACKGROUND));
	grad->setContentSize(getContentSize());
	addChild(grad);
	
	// Buttons
	std::vector<Node *> buttons;

	buttons.push_back(ButtonDef("MAP", "ui/map.png")); // Set state to map
	buttons.push_back(ButtonDef("SPELLBOOK", "ui/spellbook.png")); // Set state to spellbook

	int num = buttons.size();
	float widthPerButton = getContentSize().width / num;
	float currentX = widthPerButton / 2.0f;
	for (ButtonDef b : buttons) {
		auto n = LoadSprite(b.imagePath);
		n->setAnchorPoint(Vec2(0.5, 0.5));
		n->setPosition(currentX, getContentSize().height/2.0f);
		addChild(n);
		auto t = Label::createWithTTF(b.label, Fonts::TITLE_FONT, Fonts::TITLE_SIZE);
		t->setPosition(currentX, 10);
		addChild(t);
		currentX += widthPerButton;
		// Add onclick...
	}

	return true;
}
