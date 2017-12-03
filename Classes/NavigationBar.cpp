//
//  NavigationBar.cpp
//  Gems
//
//  Created by Sunil Patel on 12/11/2015.
//
//

#include "NavigationBar.hpp"
#include "ClickMe.hpp"
#include "SoundManager.hpp"
#include "ImageManager.hpp"
#include "GameController.hpp"
#include "Strings.hpp"
#include "Constants.h"
#include "Shaders.hpp"
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
	buttons.push_back({("ui.MAP"), "icons/map.png", kStateMap}); // Set state to map
	if (! spells.empty()) {
		buttons.push_back({("ui.SPELLBOOK"), "icons/spellbook.png", kStateSpellbook}); // Set state to spellbook
	}
	if (SaveData::isLevelComplete("10")) {
		buttons.push_back({("ui.ARENA"), "icons/arena.png", kStateArena}); // Set state to arena
	}
	/*
	buttons.push_back({("ui.OPTIONS"), "icons/settings.png", kStateOptions}); // Set state to options
	*/
	int num = buttons.size();
	float widthPerButton = getContentSize().width / num;
	float currentX = widthPerButton / 2.0f;
	//buttonPositions.clear();
	for (ButtonDef b : buttons) {
		auto n = LoadSprite(b.imagePath);
		n->setScale(0.8);
		n->setAnchorPoint(Vec2(0.5, 0.5));
		n->setPosition(currentX, getContentSize().height/2.0f - 3);
		//buttonPositions[b.label] = n->getPosition();
		if (! focus.empty() && focus == b.label) {
			auto clickMe = ClickMe::create();
			clickMe->setPosition(n->getPosition());
			buttonHolder->addChild(clickMe);
			focus = "";
		}
		buttonHolder->addChild(n);
		
		if (b.state == GameController::get()->getState()) {
			GLProgram *shader = Shaders::bright();
			GLProgramState *state = GLProgramState::create(shader);
			n->setGLProgramState(state);
		}
		
		//auto t = Label::createWithTTF(_(b.label), Fonts::TEXT_FONT, Fonts::TEXT_SIZE);
		//t->setTextColor(Color4B::BLACK);
		//t->setPosition(currentX, 10);
		//buttonHolder->addChild(t);
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
	auto ratio = getContentSize().height / sprite->getContentSize().height;
	sprite->setAnchorPoint(Vec2(0.5, 0));
	sprite->setScale(3, ratio);
	sprite->setPosition(Vec2(visibleSize.width/2, 0));
	//	sprite->setScale(ui_scale);
	addChild(sprite);
	
	buttonHolder = Layer::create();
	addChild(buttonHolder);
	
	resetButtons();

	return true;
}
