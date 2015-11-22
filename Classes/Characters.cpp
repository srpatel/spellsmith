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
	
	buff->icon = Sprite::createWithSpriteFrameName("buffs/mudshield.png");
	buff->icon->retain();
	
	buff->sprite = Sprite::createWithSpriteFrameName("spells/mudshield.png");
	buff->sprite->retain();
	
	buff->sprite->setAnchorPoint(Vec2(0.5, 0));
	buff->turns = -1; // -1 = forever, n = lasts n more turns
	buff->charges = 2; // -1 = infinite, n = n charges remaining
	
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