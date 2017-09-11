//
//  Characters.cpp
//  GemGame
//
//  Created by Sunil Patel on 08/11/2015.
//
//

#include "Characters.hpp"
#include "SoundManager.hpp"
#include "GameScene.hpp"


#if DESKTOP
#define EASY_MODE 1
#else
#define EASY_MODE 0
#endif

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

void Character::addBuff(Buff *buff, bool apply) {
	Buff *existing = getBuffByType(buff->type);
	if (existing != nullptr) {
		// remove the old one first (to-do, only remove if the new one is better?)
		removeBuff(existing);
	}
	buffs.push_back(buff);
	if (apply)
		buff->apply(this);
	updateBuffs();
}

int Character::heal(int amt, Color3B c, bool actualOnly) {
	Game *game = Game::get();
	int max_amt = max_health - health;
	amt = MIN(max_amt, amt);
	health += amt;
	if (! actualOnly) {
		ui_health += amt;
		flash(c);
		if (game) {
			game->updateHealthBars();
			game->scenery->addTextWisp(this, std::string("+") + ToString(amt), Color3B::GREEN);
		}
	}
	return amt;
}

void Character::damageEffect(int amt, Color3B c) {
	Game *game = Game::get();
	ui_health -= amt;
	flash(c);
	if (game) {
		game->scenery->addTextWisp(this, std::string("-") + ToString(amt), Color3B::RED);
	}
}

void Character::flash(Color3B c) {
	Color3B original = sprite->getColor();
	auto f = Sequence::create(
		EaseIn::create(TintTo::create(0.2f, c), 0.5),
		EaseOut::create(TintTo::create(0.2f, original), 0.5),
		nullptr);
	sprite->runAction(f);
}

Character::~Character() {
	for (Buff *b : buffs) {
		delete b;
	}
	buffs.clear();
}

void Character::removeBuff(Buff *existing) {
	auto position = std::find(buffs.begin(), buffs.end(), existing);
	if (position != buffs.end()) {
		buffs.erase(position);
	}
	
	existing->remove(this, true);
	// delete existing;
	
	updateBuffs();
}

void Character::updateBuffs() {
	buffHolder->removeAllChildren();
	float totalWidth = 15 * buffs.size() + 10 * (buffs.size() - 1);
	float x = totalWidth/2, y = 0;
	for (Buff *b : buffs) {
		if (! b->icon.empty()) {
			Sprite *s = LoadSprite(b->icon);
			s->setAnchorPoint(Vec2(0.5, 0.5));
			buffHolder->addChild(s);
			// also draw num charges
			s->setPosition(x += 25, y);
		}
	}
}

void Character::clearBuffs() {
	std::vector<Buff *> toDelete{buffs};
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

float Character::die() {
	spine::SkeletonAnimation *skeleton = (spine::SkeletonAnimation *) sprite;
	spTrackEntry *e = skeleton->addAnimation(0, "die", false);
	if (type == Humanoid) {
		SoundManager::get()->humanoidDeath();
	} else {
		SoundManager::get()->animalDeath();
	}
	return e->endTime;
}

Enemy::Enemy(Monster *m, int index) {
	this->index = index;
	monster = m;
	attack_clock = monster->attack_frequency;
#if EASY_MODE
	max_health = 20;
#else
	max_health = monster->hp;
#endif
	health = max_health;
	ui_health = max_health;
	is_skeleton = true;
	sprite = monster->makeSkeleton();
	sprite->setCascadeOpacityEnabled(true);
	type = Humanoid;
	if (monster->getSkeletonName() == "dog") {
		type = Animal;
	}
	projectile_height = sprite->getContentSize().height / 2.0;
	sprite->retain();
	buffHolder = Layer::create();
	sprite->addChild(buffHolder);
	
	projectile_height = 81;
	
	if (monster->getSkeletonName() == "dog") {
		head_offset = {-30, 60};
	} else if (monster->getSkeletonName() == "shaman") {
		head_offset = {-4, 115};
	} else if (monster->getSkeletonName() == "goblin_king") {
		head_offset = {25, 150};
	} else {
		head_offset = {-18, 85};
	}
}
Enemy::~Enemy() {
	sprite->autorelease();
}
std::map<std::string, Vec2> Enemy::getOffsets() {
	return monster->offsets;
}

Wizard::Wizard() {
	offsets["proj_from"] = Vec2(90, 0);
	offsets["proj_to"] = Vec2(-30, 0);
	type = Humanoid;
}
std::map<std::string, Vec2> Wizard::getOffsets() {
	return offsets;
}
