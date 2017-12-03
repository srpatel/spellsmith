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
#include "ClickMe.hpp"

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
	addChild(nodesHolder, 1);
	
	for (RoundDef * r : LevelManager::get()->getRoundDefinitions()) {
		// Make a node and add it!
		auto n = LoadSprite("map/node_red.png");
		n->setAnchorPoint(Vec2(0.5, 0.5));
		Vec2 pos(r->x, r->y);
		pos *= 0.4;
		pos -= getContentSize() / 2;
		n->setPosition(pos);
		nodes[r] = n;
		nodesHolder->addChild(n);
	}
	
	refreshNodes();
	
	return true;
}

void MapScroll::refreshNodes() {
	int numVisible = 0;
	Sprite *onlyVisible = nullptr;
	for (auto n : nodes) {
		bool completed =  SaveData::isLevelComplete(n.first->name);
		auto sprite = n.second;
		auto frame = LoadSpriteFrame(completed ?
			"map/node_green.png" :
			"map/node_red.png");
		sprite->setSpriteFrame(frame);
		
		// Is the dependency completed?
		auto depends = n.first->depends;
		bool isVisible = completed || depends.empty() || SaveData::isLevelComplete(depends);
		sprite->setVisible(isVisible);
		if (isVisible) {
			numVisible++;
			onlyVisible = sprite;
		}
	}
	
	removeChildByTag(1);
	if (numVisible == 1) {
		// add a click me
		auto clickMe = ClickMe::create();
		clickMe->setTag(1);
		clickMe->setPosition(onlyVisible->getPosition());
		addChild(clickMe);
	}
}
