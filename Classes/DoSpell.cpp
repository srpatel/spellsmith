//
//  DoSpell.cpp
//  Gems
//
//  Created by Sunil Patel on 29/04/2016.
//
//

#include "DoSpell.hpp"
#include "Characters.hpp"

#define IF_SPELL(s) if (strcmp(spell->getRawName().c_str(), #s) == 0)

class AmountGenerator {
public:
	inline static int between(int min, int max) {
		return (std::rand()%(max-min)) + min;
	}
};

#define HEAL(_n_) {auto amt = _n_; game->wizard->ui_health += amt;\
game->wizard->health += amt;\
if (game->wizard->health > game->wizard->max_health) {\
	game->wizard->ui_health = game->wizard->max_health;\
	game->wizard->health = game->wizard->max_health;\
}}

void DoSpell::run(Game *game, Spell *spell, Chain *chain) {
	IF_SPELL(fireball) {
		// deal 10 damage
		game->makeProjectile(
			game->wizard,
			game->enemies[game->currentEnemy], 10, Color3B::RED);
	} else IF_SPELL(mud_shield) {
		// block next 2 attacks
		Buff *shield = game->wizard->getBuffByType(BuffType::BARRIER);
	 
		if (shield == nullptr) {
			// give us mudshield_buff, and animate it in using ga.
			auto shield = Buff::createMudshield();
			
			// put it in a good place and add it
			shield->sprite->setPosition(game->wizard->sprite->getPosition() + Vec2(75, 0) + game->scenery->getPosition());
			shield->sprite->setOpacity(0);
			game->addChild(shield->sprite);
			
			// fade it in
			auto fadeIn = FadeIn::create(0.2);
			
			game->wizard->addBuff(shield);
			
			shield->sprite->runAction(fadeIn);
		} else {
			shield->charges = 2;
		}
	} else IF_SPELL(forest_breeze) {
		// gain 5
		HEAL(5);
		game->updateHealthBars();
	} else IF_SPELL(lightning_bolt) {
		// deal 1-12
		int amount = AmountGenerator::between(1, 12);
		game->makeProjectile(
							 game->wizard,
							 game->enemies[game->currentEnemy], amount, Color3B::YELLOW);
	} else IF_SPELL(chill) {
		// slow enemy by 2 turns
		game->enemies[game->currentEnemy]->attack_clock += 2;
		game->hud->updateAttackClocks();
	} else IF_SPELL(firewisp) {
		// deal 6 damage
		game->makeProjectile(
							 game->wizard,
							 game->enemies[game->currentEnemy], 6, Color3B::RED);
	} else IF_SPELL(healstrike) {
		// gain 5, deal 5 damage
		HEAL(5);
		game->updateHealthBars();
		game->makeProjectile(
							 game->wizard,
							 game->enemies[game->currentEnemy], 5, Color3B::RED);
	} else IF_SPELL(volcanic) {
		// deal 10 damage to everyone
		game->wizard->ui_health -= 10;
		game->wizard->health -= 10;
		for (Enemy *e : game->enemies) {
			e->ui_health -= 10;
			e->health -= 10;
		}
		game->updateHealthBars();
	} else IF_SPELL(crystalise) {
		// heal 3, create 3 crystal gems
		HEAL(3);
		game->grid->createRandomCrystalGems(3, chain);
		game->updateHealthBars();
	} else IF_SPELL(zap) {
		// deal 10 to a random enemy
		int i = game->getNextAliveEnemy(rand() % game->enemies.size());
		game->makeProjectile(
							 game->wizard,
							 game->enemies[i], 10, Color3B::YELLOW);
	} else IF_SPELL(smelt) {
		// deal 6 damage, create 1 crystal gem
		game->makeProjectile(
							 game->wizard,
							 game->enemies[game->currentEnemy], 6, Color3B::RED);
		game->grid->createRandomCrystalGems(1, chain);
	} else IF_SPELL(ice_bolt) {
		// deal 3 damage, 50% chance to freeze 2
		game->makeProjectile(
							 game->wizard,
							 game->enemies[game->currentEnemy], 3, Color3B::BLUE);
		if (rand() % 2) {
			game->enemies[game->currentEnemy]->addBuff(
												   Buff::createFreeze(2)
												   );
		}
	} else IF_SPELL(stun_strike) {
		// stun
		game->enemies[game->currentEnemy]->addBuff(Buff::createStun());
	} else IF_SPELL(earthquake) {
		// 5 damage to each enemy. 50% chance to stun.
		for (Enemy *e : game->enemies) {
			e->ui_health -= 5;
			e->health -= 5;
			if (rand() % 2) {
				e->addBuff(Buff::createStun());
			}
		}
		game->updateHealthBars();
	} else {
		LOG("No spell definition found for %s.\n", spell->getRawName().c_str());
	}
}
