//
//  Characters.cpp
//  GemGame
//
//  Created by Sunil Patel on 08/11/2015.
//
//

#include "Characters.hpp"


#if DESKTOP
#define EASY_MODE 1
#else
#define EASY_MODE 0
#endif

bool BuffComparator (Buff *left, Buff *right) {
	return left->priority <= right->priority;
}

Buff *Buff::createMudshield() {
	auto buff = new Buff;
	buff->type = BuffType::BARRIER;
	buff->positive = true;
	
	buff->icon = "buffs/mudshield.png";
	
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
	
	buff->icon = "buffs/frozen.png";
	
	buff->sprite = nullptr;
	
	buff->turns = -1; // -1 = forever, n = lasts n more turns
	buff->charges = amount; // -1 = infinite, n = n charges remaining
	
	buff->priority = 1;
	
	return buff;
}

Buff *Buff::createStun(){
	auto buff = new Buff;
	buff->type = BuffType::STUN;
	buff->positive = true;
	
	buff->icon = "buffs/stun.png";
	
	buff->sprite = nullptr;
	
	buff->turns = -1; // -1 = forever, n = lasts n more turns
	buff->charges = 1; // -1 = infinite, n = n charges remaining
	
	buff->priority = 1;
	
	return buff;
}

Buff *Buff::createKingsCourt(){
	auto buff = new Buff;
	buff->type = BuffType::KINGS_COURT;
	buff->positive = true;
	
	buff->icon = "buffs/kingscourt.png";
	
	buff->sprite = nullptr;
	
	buff->turns = 2; // -1 = forever, n = lasts n more turns
	buff->charges = -1; // -1 = infinite, n = n charges remaining
	
	buff->priority = 1;
	
	return buff;
}

Buff *Buff::createFury(){
	auto buff = new Buff;
	buff->type = BuffType::FURY;
	buff->positive = true;
	
	buff->icon = "buffs/fury.png";
	
	buff->sprite = nullptr;
	
	buff->turns = 2; // -1 = forever, n = lasts n more turns
	buff->charges = -1; // -1 = infinite, n = n charges remaining
	
	buff->priority = 1;
	
	return buff;
}

Buff *Buff::createPhasing(){
	auto buff = new Buff;
	buff->type = BuffType::PHASING;
	buff->positive = true;
	
	buff->icon = "buffs/phase.png";
	
	buff->sprite = nullptr;
	
	buff->turns = 1; // -1 = forever, n = lasts n more turns
	buff->charges = -1; // -1 = infinite, n = n charges remaining
	
	buff->priority = 1;
	
	return buff;
}

Buff::~Buff() {
	if (sprite)
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
	if (buff->type == BuffType::PHASING) {
		auto f = Sequence::create(
			FadeTo::create(0.2f, 125),
			DelayTime::create(1.0f),
			CallFunc::create([this]() {
				// if we still have phasing, do nothing.
				// otherwise, fade out.
				Buff *existing = getBuffByType(BuffType::PHASING);
				if (! existing) {
					auto f = FadeTo::create(0.5f, 255);
					sprite->runAction(f);
				}
			}),
			nullptr);
		f->setTag(BuffType::PHASING);
		sprite->stopActionByTag(BuffType::PHASING);
		sprite->runAction(f);
	} else if (buff->type == BuffType::FURY) {
		auto f = Sequence::create(
			TintTo::create(0.2f, 255, 125, 125),
			DelayTime::create(1.0f),
			CallFunc::create([this]() {
				// if we still have phasing, do nothing.
				// otherwise, fade out.
				Buff *existing = getBuffByType(BuffType::FURY);
				if (! existing) {
					auto f = TintTo::create(0.5f, 255, 255, 255);
					sprite->runAction(f);
				}
			}),
			nullptr);
		f->setTag(BuffType::FURY);
		sprite->stopActionByTag(BuffType::FURY);
		sprite->runAction(f);
	}
	updateBuffs();
}

void Character::removeBuff(Buff *existing) {
	if (existing->type == BuffType::PHASING) {
		auto a = sprite->getActionByTag(BuffType::PHASING);
		if (! a) {
			auto f = FadeTo::create(0.5f, 255);
			f->setTag(BuffType::PHASING);
			sprite->runAction(f);
		}
	} else if (existing->type == BuffType::FURY) {
		auto a = sprite->getActionByTag(BuffType::FURY);
		if (! a) {
			auto f = TintTo::create(0.5f, 255, 255, 255);
			f->setTag(BuffType::FURY);
			sprite->runAction(f);
		}
	}
	
	auto position = std::find(buffs.begin(), buffs.end(), existing);
	if (position != buffs.end()) {
		buffs.erase(position);
	}
	delete existing;
	
	updateBuffs();
}

void Character::updateBuffs() {
	buffHolder->removeAllChildren();
	float totalWidth = 15 * buffs.size() + 10 * (buffs.size() - 1);
	float x = totalWidth/2, y = 0;
	for (Buff *b : buffs) {
		Sprite *s = LoadSprite(b->icon);
		s->setAnchorPoint(Vec2(0.5, 0.5));
		buffHolder->addChild(s);
		// also draw num charges
		s->setPosition(x += 25, y);
	}
}

void Character::clearBuffs() {
	std::vector<Buff *> toDelete;
	for (Buff *b : buffs) {
		toDelete.push_back(b);
	}
	for (Buff *b : toDelete) {
		removeBuff(b);
	}
	toDelete.clear();
}


void Character::tickBuffs() {
	std::vector<Buff *> toDelete;
	for (Buff *b : buffs) {
		if (b->turns > 0) {
			b->turns--;
			if (b->turns == 0) {
				toDelete.push_back(b);
			}
		}
	}
	for (Buff *b : toDelete) {
		removeBuff(b);
	}
	toDelete.clear();
}

Enemy::Enemy(Monster *m, int index) {
	this->index = index;
	monster = m;
	attack_clock = monster->attack_frequency;
#if EASY_MODE
	max_health = 10;
#else
	max_health = monster->hp;
#endif
	health = max_health;
	ui_health = max_health;
	is_skeleton = true;
	sprite = monster->makeSkeleton();
	projectile_height = sprite->getContentSize().height / 2.0;
	sprite->retain();
	buffHolder = Layer::create();
	sprite->addChild(buffHolder);
}
Enemy::~Enemy() {
	sprite->autorelease();
}
