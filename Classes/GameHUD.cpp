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
	
	arrow = LoadSprite("ui/arrow.png");
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
		auto monsterName = Label::createWithTTF(e->monster->getName(), Fonts::TEXT_FONT, Fonts::SMALL_SIZE);
		monsterName->setColor(Color3B::BLACK);
		monsterName->setAnchorPoint(Vec2(0, 0.5));
		monsterName->setPosition(15, (enemies->size() - i - 1) * heightPerItem + heightPerItem/2);
		addChild(monsterName);
		
		float x = getContentSize().width - 85;
		float y = (enemies->size() - i - 1) * heightPerItem + heightPerItem/2;
		
		auto attackclocksprite_red = LoadSprite("ui/attack_counter_red.png");
		auto attackclocksprite_black = LoadSprite("ui/attack_counter_black.png");
		attackclocksprite_red->setAnchorPoint(Vec2(0.5, 0.5));
		attackclocksprite_black->setAnchorPoint(Vec2(0.5, 0.5));
		attackclocksprite_red->setPosition(x, y);
		attackclocksprite_black->setPosition(x, y);
		attackclock_reds[i] = attackclocksprite_red;
		addChild(attackclocksprite_black);
		addChild(attackclocksprite_red);
		
		auto attackclock = Label::createWithTTF(ToString(e->attack_clock), Fonts::NUMBER_FONT, 10);
		attackclock->setHorizontalAlignment(TextHAlignment::CENTER);
		attackclock->setColor(Color3B::WHITE);
		attackclock->setAnchorPoint(Vec2(0.5, 0.5));
		attackclock->setPosition(x - 1, y);
		addChild(attackclock);
		attackclocks[i] = attackclock;
		
		auto hpBar = HealthBar::create();
		hpBar->setAnchorPoint(Vec2(1, 0.5));
		hpBar->setPosition(getContentSize().width - 35, (enemies->size() - i - 1) * heightPerItem + heightPerItem/2);
		hpBar->setHealths(e->ui_health, e->max_health);
		addChild(hpBar);
		healthbars[i] = hpBar;
		
		i++;
	}
	updateAttackClocks();
	
	// This shouldn't happen...but it has crashed!
	arrow->removeFromParent();
	
	addChild(arrow);
	setSelected(0);
}
void GameHUD::updateHealthBars() {
	if (enemies)
		for (int i = 0; i < enemies->size(); i++) {
			auto c = enemies->at(i);
			healthbars[i]->setHealths(c->ui_health, c->max_health);
		}
}
void GameHUD::updateAttackClocks() {
	for (int i = 0; i < enemies->size(); i++) {
		auto c = enemies->at(i);
		attackclock_reds[i]->setVisible(c->attack_clock == 0 && ! c->dead());
		if (c->dead() || c->attack_clock == 0) {
			attackclocks[i]->setString(std::string(""));
		} else {
			attackclocks[i]->setString(ToString(c->attack_clock));
		}
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
	
	auto grad = LayerColor::create();
	grad->initWithColor(Color4B(67, 61, 54, 255));
	grad->setPosition(bg->getContentSize()/-2 + Size(2, 2));
	grad->setContentSize(bg->getContentSize() - Size(4, 4));
	addChild(grad);
	
	text = Label::createWithTTF("100/100", Fonts::NUMBER_FONT, 8);
	text->setHorizontalAlignment(TextHAlignment::CENTER);
	text->setColor(Color3B::WHITE);
	text->setAnchorPoint(Vec2(0.5, 0.5));
	
	addChild(hp);
	addChild(bg);
	addChild(text);
	
	return true;
}

void HealthBar::setHealths(int current, int max) {
	if (current <= 0) {
		text->setString("");
	} else {
		text->setString(ToString(current) + "/" + ToString(max));
	}
	float perc = MAX(0, (float) current / max);
	auto target_width = perc * (getContentSize().width - 4);
	hp->setScaleX(target_width / hp->getContentSize().width);
}
