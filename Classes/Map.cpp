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
	Layer *nodesHolder = Layer::create();
	addChild(nodesHolder);
	
	nodesHolder->removeAllChildren();
	/*for (EventListener *l : listeners) {
		_eventDispatcher->removeEventListener(l);
	}
	listeners.clear();*/
	
	//float offset_y = 50;
	for (RoundDef * r : LevelManager::get()->getRoundDefinitions()) {
		// Make a node and add it!
		auto n = LoadSprite("map/node_red.png");
		n->setAnchorPoint(Vec2(0.5, 0.5));
		Vec2 pos(r->x, r->y);
		pos *= scale;
		pos -= getContentSize() / 2;
		n->setPosition(pos);
		nodes[r] = n;
		nodesHolder->addChild(n);
		
		auto onclick = EventListenerTouchOneByOne::create();
		onclick->setSwallowTouches(true);
		onclick->onTouchBegan = [this, r](Touch* touch, Event* event) -> bool {
			auto bounds = event->getCurrentTarget()->getBoundingBox();
			bounds.origin += getPosition();
			
			// Can only click on me if the dependency is completed
			auto depends = r->depends;
			if (!depends.empty() && !SaveData::isLevelComplete(depends)) {
				return false;
			}
			
			if (bounds.containsPoint(touch->getLocation())){
				PLAY_SOUND( kSoundEffect_UISelect );
				GameController::get()->showPreLevelDialog(r);
				return true;
			}
			
			return false; // if you are consuming it
		};
		_eventDispatcher->addEventListenerWithSceneGraphPriority(onclick, n);
		//listeners.push_back(onclick);
	}
	
	refreshNodes();
	
	return true;
}

void MapScroll::refreshNodes() {
	for (auto n : nodes) {
		bool completed =  SaveData::isLevelComplete(n.first->name);
		auto sprite = n.second;
		auto frame = LoadSpriteFrame(completed ?
			"map/node_green.png" :
			"map/node_red.png");
		sprite->setSpriteFrame(frame);
		
		// Is the dependency completed?
		auto depends = n.first->depends;
		sprite->setVisible(completed || depends.empty() || SaveData::isLevelComplete(depends));
	}
}
