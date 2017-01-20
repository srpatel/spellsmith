//
//  MapScreen.cpp
//  Gems
//
//  Created by Sunil Patel on 16/01/2017.
//
//

#include "Level.hpp"
#include "MapScreen.hpp"
#include "ui/CocosGUI.h"
#include "Strings.hpp"
#include "Constants.h"
#include "GameController.hpp"
#include "SaveData.hpp"

bool MapScreen::init() {
	if ( !Layer::init() ) {
		return false;
	}
	
	auto size = Director::getInstance()->getVisibleSize();
	
	// If you click on a level, go to that level.
	// If you click back, go to main menu.
	// If you click on spell book, go to spell book.
	auto button = ui::Button::create("ui/button_up.png", "ui/button_down.png", "ui/button_down.png", TEXTURE_TYPE);
	button->setTitleFontName(Fonts::TEXT_FONT);
	button->setTitleFontSize(Fonts::TEXT_SIZE);
	button->setPosition(Vec2(size.width/2, size.height/2));
	button->setTitleText _("ui.ARENA");
	button->addTouchEventListener([](Ref* pSender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			// For now, from scratch always!
			GameController::get()->startArena();
		}
	});
	this->addChild(button);
	
	// todo: coordinates are such on the original image
	// scale map so that width is correct, and then scale offsets by that amount too
	nodes = Layer::create();
	addChild(nodes);
	
	return true;
}

void MapScreen::refreshNodes() {
	nodes->removeAllChildren();
	for (EventListener *l : listeners) {
		_eventDispatcher->removeEventListener(l);
	}
	listeners.clear();
	
	float offset_y = 50;
	for (RoundDef * r : LevelManager::get()->getRoundDefinitions()) {
		// Make a node and add it!
		auto n =
			SaveData::isLevelComplete(r->name) ?
			LoadSprite("icons/mapnodedone.png") :
			LoadSprite("icons/mapnode.png");
		n->setAnchorPoint(Vec2(0.5, 0.5));
		n->setPosition(r->x, r->y + offset_y);
		nodes->addChild(n);
		
		auto t = Label::createWithTTF(r->name, Fonts::TEXT_FONT, Fonts::TEXT_SIZE);
		t->setTextColor(Color4B::WHITE);
		t->setPosition(n->getPosition());
		nodes->addChild(t);
		
		auto onclick = EventListenerTouchOneByOne::create();
		onclick->setSwallowTouches(true);
		onclick->onTouchBegan = [this, r](Touch* touch, Event* event) -> bool {
			auto bounds = event->getCurrentTarget()->getBoundingBox();
			
			if (bounds.containsPoint(touch->getLocation())){
				GameController::get()->showPreLevelDialog(r);
				return true;
			}
			
			return false; // if you are consuming it
		};
		_eventDispatcher->addEventListenerWithSceneGraphPriority(onclick, n);
		listeners.push_back(onclick);
	}
}
