//
//  Buff.cpp
//  Gems
//
//  Created by Sunil Patel on 19/08/2017.
//
//

#include "Buff.hpp"
#include "Characters.hpp"
#include "Projectiles.hpp"
#include "SoundManager.hpp"
#include "GameScene.hpp"

bool BuffComparator (Buff *left, Buff *right) {
	return left->priority <= right->priority;
}

void Buff::apply(Character *c) {
	applied = true;
	_apply(c);
	if (queuedRemoved) {
		remove(c, willDeleteSelf);
	}
}

void Buff::remove(Character *c, bool deleteSelf) {
	if (!applied) {
		// if the buff isn't applied yet...remove it straight after it gets added
		queuedRemoved = true;
		willDeleteSelf = deleteSelf;
	} else {
		_remove(c);
		if (deleteSelf) {
			delete this;
		}
	}
}

class EmptyBuff : public Buff {
	void _apply(Character *c) override { }
	void _remove(Character *c) override { }
};


Buff *Buff::createMudshield() {
	auto buff = new EmptyBuff;
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
	auto buff = new EmptyBuff;
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
	class StunBuff : public Buff {
		Node *top, *bottom;
		void _apply(Character *c) override {
			auto char_scale = Game::get()->scenery->char_scale;
			Vec2 pos = c->sprite->getPosition() + c->head_offset * char_scale;
			
			top = AnimStarsTop::create(pos, 0.8f, nullptr, true);
			bottom = AnimStarsBottom::create(pos, 0.8f, nullptr, true);
			
			auto fadeIn = FadeIn::create(1);
			top->runAction(fadeIn);
			bottom->runAction(fadeIn->clone());
			
			// enemy is at 50-100, so we use 25 and 125
			c->sprite->getParent()->addChild(top, 125);
			c->sprite->getParent()->addChild(bottom, 25);
		}
		void _remove(Character *c) override {
			auto seq = Sequence::create(
				DelayTime::create(0.5),
				FadeOut::create(1),
				RemoveSelf::create(),
				nullptr
			);
			top->runAction(seq);
			bottom->runAction(seq->clone());
		}
	};
	auto buff = new StunBuff;
	buff->type = BuffType::STUN;
	buff->positive = true;
	
	buff->icon = ""; //"buffs/stun.png";
	
	buff->sprite = nullptr;
	
	buff->turns = -1; // -1 = forever, n = lasts n more turns
	buff->charges = 1; // -1 = infinite, n = n charges remaining
	
	buff->priority = 1;
	
	return buff;
}

Buff *Buff::createFocus(){
	auto buff = new EmptyBuff;
	buff->type = BuffType::FOCUS;
	buff->positive = true;
	
	buff->icon = "buffs/focus.png";
	
	buff->sprite = nullptr;
	
	buff->turns = -1; // -1 = forever, n = lasts n more turns
	buff->charges = -1; // -1 = infinite, n = n charges remaining
	
	buff->priority = 1;
	
	return buff;
}

Buff *Buff::createSpellFocus(){
	class FocusBuff : public Buff {
		Node *glow;
		void _apply(Character *c) override {
			auto char_scale = Game::get()->scenery->char_scale;
			Vec2 pos = c->sprite->getPosition();
			pos.y += char_scale * 75;
			
			glow = AnimBlueGlow::create(pos, 2 * char_scale, nullptr, true);
			
			auto fadeIn = FadeIn::create(1);
			glow->runAction(fadeIn);
			
			// enemy is at 50-100, so we use 25 and 125
			c->sprite->getParent()->addChild(glow, 125);
		}
		void _remove(Character *c) override {
			auto seq = Sequence::create(
				DelayTime::create(0.5),
				FadeOut::create(1),
				RemoveSelf::create(),
				nullptr
			);
			glow->runAction(seq);
		}
	};
	auto buff = new FocusBuff;
	buff->type = BuffType::SPELL_FOCUS;
	buff->positive = true;
	
	buff->icon = "";
	
	buff->sprite = nullptr;
	
	buff->turns = 6; // -1 = forever, n = lasts n more turns
	buff->charges = -1; // -1 = infinite, n = n charges remaining
	
	buff->priority = 1;
	
	return buff;
}

Buff *Buff::createChargeBolt(){
	auto buff = new EmptyBuff;
	buff->type = BuffType::CHARGE_BOLT;
	buff->positive = true;
	
	buff->icon = "";
	
	buff->sprite = nullptr;
	
	buff->turns = -1; // -1 = forever, n = lasts n more turns
	buff->charges = -1; // -1 = infinite, n = n charges remaining
	
	buff->priority = 1;
	
	return buff;
}

Buff *Buff::createKingsCourt(){
	auto buff = new EmptyBuff;
	buff->type = BuffType::KINGS_COURT;
	buff->positive = true;
	
	buff->icon = "";
	
	buff->sprite = nullptr;
	
	buff->turns = 2; // -1 = forever, n = lasts n more turns
	buff->charges = -1; // -1 = infinite, n = n charges remaining
	
	buff->priority = 1;
	
	return buff;
}

Buff *Buff::createFury(){
	class FuryBuff : public Buff {
		void _apply(Character *c) override {
			auto f = Sequence::create(
				TintTo::create(0.2f, 255, 125, 125),
				DelayTime::create(1.0f),
				CallFunc::create([this, c]() {
					// if we still have phasing, do nothing.
					// otherwise, fade out.
					Buff *existing = c->getBuffByType(BuffType::FURY);
					if (! existing) {
						auto f = TintTo::create(0.5f, 255, 255, 255);
						c->sprite->runAction(f);
					}
				}),
				nullptr);
			f->setTag(BuffType::FURY);
			c->sprite->stopActionByTag(BuffType::FURY);
			c->sprite->runAction(f);
		}
		void _remove(Character *c) override {
			auto a = c->sprite->getActionByTag(BuffType::FURY);
			if (! a) {
				auto f = TintTo::create(0.5f, 255, 255, 255);
				f->setTag(BuffType::FURY);
				c->sprite->runAction(f);
			}
		}
	};
	auto buff = new FuryBuff;
	buff->type = BuffType::FURY;
	buff->positive = true;
	
	buff->icon = ""; //"buffs/fury.png";
	
	buff->sprite = nullptr;
	
	buff->turns = 2; // -1 = forever, n = lasts n more turns
	buff->charges = -1; // -1 = infinite, n = n charges remaining
	
	buff->priority = 1;
	
	return buff;
}

Buff *Buff::createPhasing(){
	class PhasingBuff : public Buff {
		void _apply(Character *c) override {
			PLAY_SOUND(kSoundEffect_SPhase);
			auto f = Sequence::create(
				FadeTo::create(0.2f, 125),
				DelayTime::create(1.0f),
				CallFunc::create([this, c]() {
					// if we still have phasing, do nothing.
					// otherwise, fade back in.
					Buff *existing = c->getBuffByType(BuffType::PHASING);
					if (! existing) {
						PLAY_SOUND(kSoundEffect_SUnphase);
						auto f = FadeTo::create(0.5f, 255);
						c->sprite->runAction(f);
					}
				}),
				nullptr);
			f->setTag(BuffType::PHASING);
			c->sprite->stopActionByTag(BuffType::PHASING);
			c->sprite->runAction(f);
		}
		void _remove(Character *c) override {
			auto a = c->sprite->getActionByTag(BuffType::PHASING);
			if (! a) {
				PLAY_SOUND(kSoundEffect_SUnphase);
				auto f = FadeTo::create(0.5f, 255);
				f->setTag(BuffType::PHASING);
				c->sprite->runAction(f);
			}
		}
	};
	auto buff = new PhasingBuff;
	buff->type = BuffType::PHASING;
	buff->positive = true;
	
	buff->icon = ""; // "buffs/phase.png";
	
	buff->sprite = nullptr;
	
	buff->turns = 1; // -1 = forever, n = lasts n more turns
	buff->charges = -1; // -1 = infinite, n = n charges remaining
	
	buff->priority = 1;
	
	return buff;
}

Buff *Buff::createPoison(){
	auto buff = new EmptyBuff;
	buff->type = BuffType::POISON;
	buff->positive = true;
	
	buff->icon = "";
	
	buff->sprite = nullptr;
	
	buff->turns = -1; // -1 = forever, n = lasts n more turns
	buff->charges = -1; // -1 = infinite, n = n charges remaining
	
	buff->priority = 1;
	
	return buff;
}

Buff::~Buff() {
	if (sprite)
		sprite->release();
}
