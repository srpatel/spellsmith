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

void GameHUD::setupMonsterList(std::vector<Enemy *> *e) {
	enemies = e;
	// Clear all children!
	removeAllChildren();
	
	// Add a list item for each enemy.
	float heightPerItem = getContentSize().height / enemies->size();
	int i = 0;
	for (Enemy *e : *enemies) {
		auto monsterName = Label::createWithTTF(e->monster->name, Fonts::TEXT_FONT, Fonts::SMALL_SIZE);
		monsterName->setColor(Color3B::BLACK);
		monsterName->setAnchorPoint(Vec2(0, 0.5));
		monsterName->setPosition(15, (enemies->size() - i - 1) * heightPerItem + heightPerItem/2);
		addChild(monsterName);
		
		auto hpBar = HealthBar::create();
		hpBar->setAnchorPoint(Vec2(1, 0.5));
		hpBar->setPosition(getContentSize().width - 35, (enemies->size() - i - 1) * heightPerItem + heightPerItem/2);
		hpBar->setPercentage(e->ui_health / (float) e->max_health);
		addChild(hpBar);
		healthbars[i] = hpBar;
		
		i++;
	}
	
	addChild(arrow);
	setSelected(0);
}
void GameHUD::updateHealthBars() {
	for (int i = 0; i < enemies->size(); i++) {
		auto c = enemies->at(i);
		healthbars[i]->setPercentage(MAX(0, c->ui_health / (float) c->max_health));
	}
}
void GameHUD::setSelected(int i) {
	float heightPerItem = getContentSize().height / enemies->size();
	arrow->setPosition(5, (enemies->size() - i - 1) * heightPerItem + heightPerItem/2);
}
GameHUD::~GameHUD() {
	arrow->autorelease();
}

bool HealthBar::init() {
	if ( !Layer::init() )
	{
		return false;
	}
	
	bg = LoadSprite("ui/healthbar.png");
	hp = LoadSprite("ui/healthpercent.png");
	hp->setAnchorPoint(Vec2(0, 0.5));
	hp->setPositionX(-bg->getContentSize().width/2 + 2);
	setContentSize(bg->getContentSize());
	
	addChild(hp);
	addChild(bg);
	
	return true;
}

void HealthBar::setPercentage(float perc) {
	hp->setScaleX(perc * 102);
}