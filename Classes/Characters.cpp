//
//  Characters.cpp
//  GemGame
//
//  Created by Sunil Patel on 08/11/2015.
//
//

#include "Characters.hpp"

bool BuffComparator (Buff *left, Buff *right) {
	return left->priority <= right->priority;
}

Buff *Buff::createMudshield() {
	auto buff = new Buff;
	buff->type = BuffType::BARRIER;
	buff->positive = true;
	
	buff->icon = LoadSprite("buffs/mudshield.png");
	buff->icon->retain();
	
	buff->sprite = LoadSprite("spells/mudshield.png");
	buff->sprite->retain();
	
	buff->sprite->setAnchorPoint(Vec2(0.5, 0));
	buff->turns = -1; // -1 = forever, n = lasts n more turns
	buff->charges = 2; // -1 = infinite, n = n charges remaining
	
	buff->priority = 1;
	
	return buff;
}

Buff *Buff::createFreeze(int amount){
	auto buff = new Buff;
	buff->type = BuffType::FREEZE;
	buff->positive = true;
	
	buff->icon = LoadSprite("buffs/mudshield.png");
	buff->icon->retain();
	
	buff->sprite = LoadSprite("spells/mudshield.png");
	buff->sprite->retain();
	
	buff->sprite->setAnchorPoint(Vec2(0.5, 0));
	buff->turns = -1; // -1 = forever, n = lasts n more turns
	buff->charges = amount; // -1 = infinite, n = n charges remaining
	
	buff->priority = 1;
	
	return buff;
}

Buff *Buff::createStun(){
	auto buff = new Buff;
	buff->type = BuffType::STUN;
	buff->positive = true;
	
	buff->icon = LoadSprite("buffs/mudshield.png");
	buff->icon->retain();
	
	buff->sprite = LoadSprite("spells/mudshield.png");
	buff->sprite->retain();
	
	buff->sprite->setAnchorPoint(Vec2(0.5, 0));
	buff->turns = -1; // -1 = forever, n = lasts n more turns
	buff->charges = 1; // -1 = infinite, n = n charges remaining
	
	buff->priority = 1;
	
	return buff;
}

Buff::~Buff() {
	icon->release();
	sprite->release();
}

Buff *Character::getBuffByType(BuffType type) {
	Buff *ret = nullptr;
	for (Buff *b : buffs) {
		if (b->type == type) {
			ret = b;
			break;
		}
	}
	return ret;
}

void Character::addBuff(Buff *buff) {
	Buff *existing = getBuffByType(buff->type);
	if (existing != nullptr) {
		// remove the old one first (to-do, only remove if the new one is better?)
		removeBuff(existing);
	}
	buffs.push_back(buff);
}

void Character::removeBuff(Buff *existing) {
	auto position = std::find(buffs.begin(), buffs.end(), existing);
	if (position != buffs.end()) {
		buffs.erase(position);
	}
	delete existing;
}

Enemy::Enemy(Monster *m, int index) {
	this->index = index;
	monster = m;
	attack_clock = monster->attack_frequency;
	max_health = monster->hp;
	health = monster->hp;
	ui_health = monster->hp;
	sprite = LoadSprite(monster->sprite_path);
	sprite->retain();
}
Enemy::~Enemy() {
	sprite->autorelease();
}