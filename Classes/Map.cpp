//
//  Map.cpp
//  Gems
//
//  Created by Sunil Patel on 24/06/2017.
//
//

#include "Map.hpp"
#include "Level.hpp"
#include "Constants.h"
#include "SoundManager.hpp"
#include "GameController.hpp"
#include "SaveData.hpp"

bool MapScroll::init() {
	if ( !Layer::init() ) {
		return false;
	}
	
	auto scale = 0.4 * Director::getInstance()->getContentScaleFactor();
	auto map = LoadLargeSprite("map.png");
	map->setScale(scale);
	setContentSize(map->getContentSize() * scale);
	addChild(map);
	
	// Add nodes!
	nodes = Layer::create();
	addChild(nodes);
	
	nodes->removeAllChildren();
	for (EventListener *l : listeners) {
		_eventDispatcher->removeEventListener(l);
	}
	listeners.clear();
	
	//float offset_y = 50;
	for (RoundDef * r : LevelManager::get()->getRoundDefinitions()) {
		// Make a node and add it!
		auto n =
			SaveData::isLevelComplete(r->name) ?
			LoadSprite("map/node_green.png") :
			LoadSprite("map/node_red.png");
		n->setAnchorPoint(Vec2(0.5, 0.5));
		Vec2 pos(r->x, r->y);
		pos *= scale;
		pos -= getContentSize() / 2;
		n->setPosition(pos);
		nodes->addChild(n);
		
		auto onclick = EventListenerTouchOneByOne::create();
		onclick->setSwallowTouches(true);
		onclick->onTouchBegan = [this, r](Touch* touch, Event* event) -> bool {
			auto bounds = event->getCurrentTarget()->getBoundingBox();
			bounds.origin += getPosition();
			
			if (bounds.containsPoint(touch->getLocation())){
				PLAY_SOUND( kSoundEffect_UISelect );
				GameController::get()->showPreLevelDialog(r);
				return true;
			}
			
			return false; // if you are consuming it
		};
		_eventDispatcher->addEventListenerWithSceneGraphPriority(onclick, n);
		listeners.push_back(onclick);
	}
	
	return true;
}
