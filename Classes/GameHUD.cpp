//
//  GameHUD.cpp
//  Gems
//
//  Created by Sunil Patel on 17/04/2016.
//
//

#include "GameHUD.hpp"
#include "Strings.hpp"
#include "Constants.h"

bool GameHUD::init() {
	if ( !Layer::init() )
	{
		return false;
	}
	
	arrow = LoadSprite("ui/redarrow.png");
	arrow->retain();
	arrow->setAnchorPoint(Vec2(0, 0.5));
	
	return true;
}

void GameHUD::setupMonsterList(std::vector<Enemy *> &enemies) {
	// Clear all children!
	removeAllChildren();
	
	// Add a list item for each enemy.
	float heightPerItem = getContentSize().height / enemies.size();
	int i = 0;
	for (Enemy *e : enemies) {
		auto monsterName = Label::createWithTTF(e->monster->name, Fonts::TEXT_FONT, Fonts::SMALL_SIZE);
		monsterName->setColor(Color3B::BLACK);
		monsterName->setAnchorPoint(Vec2(0, 0.5));
		monsterName->setPosition(15, i++ * heightPerItem + heightPerItem/2);
		addChild(monsterName);
	}
	
	addChild(arrow);
	arrow->setPosition(5, heightPerItem/2);
}

GameHUD::~GameHUD() {
	arrow->autorelease();
}
