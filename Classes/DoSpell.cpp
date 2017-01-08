//
//  DoSpell.cpp
//  Gems
//
//  Created by Sunil Patel on 29/04/2016.
//
//

#include "DoSpell.hpp"
#include "Characters.hpp"

#define IF_SPELL(s) else if (strcmp(spell->getRawName().c_str(), #s) == 0)

class AmountGenerator {
public:
	inline static int between(int min, int max) {
		return (std::rand()%(max-min)) + min;
	}
};

#define D(_n_) (_n_ * damageModifier)
#define D_BETWEEN(_lo_, _hi_) (AmountGenerator::between(_lo_, _hi_) * damageModifier)

#define CRYSTAL(_n_) game->grid->createRandomCrystalGems(_n_, chain);
#define HEAL(_n_) {\
	auto amt = _n_; \
	game->wizard->ui_health += amt;\
	game->wizard->health += amt;\
	if (game->wizard->health > game->wizard->max_health) {\
		game->wizard->ui_health = game->wizard->max_health;\
		game->wizard->health = game->wizard->max_health;\
	}\
	game->updateHealthBars();\
	game->scenery->addTextWisp(game->wizard, std::string("+") + ToString(amt), Color3B::GREEN);\
	}
#define PROJ(_n_, _t_) game->makeProjectile(\
	game->wizard, \
	game->enemies[game->currentEnemy], \
	_n_,\
	_t_);
#define PROJ_RAND(_n_, _t_) { int i = game->getNextAliveEnemy(rand() % game->enemies.size());\
	game->makeProjectile(\
	game->wizard, \
	game->enemies[i], \
	_n_,\
	_t_); }

void DoSpell::run(Game *game, Spell *spell, Chain *chain, bool allowRepeats) {
	// Modifiers
	// Rage
	int damageModifier = 1;
	if (game->wizard->getBuffByType(BuffType::FURY)) {
		damageModifier = 2;
	}
	// King's Court
	if (allowRepeats && game->wizard->getBuffByType(BuffType::KINGS_COURT)) {
		// Repeat the spell twice more!
		run(game, spell, chain, false);
		run(game, spell, chain, false);
	}

	if (1 == 0);
	IF_SPELL(fertilise) {
		// Replace all green gems with crystal
		game->grid->makeCrystalsOverGemsOfType( GemType::EARTH, chain );
	}
	IF_SPELL(fire_cleanse) {
		// Destroy all red gems and deal 2 damage for each
		int n = game->grid->destroyGemsOfType( GemType::FIRE, chain );
		PROJ( D(n), Color3B::RED );
	}
	IF_SPELL(fountain_of_youth) {
		// Destroy all blue gems and heal 1 for each
		int n = game->grid->destroyGemsOfType( GemType::WATER, chain );
		HEAL( n );
	}
	IF_SPELL(focus) {
		// deal extra damage with chains for 4 turns
		game->wizard->addBuff( Buff::createFocus() );
	}
	IF_SPELL(channel) {
		// cast the next spell three times
		game->wizard->addBuff( Buff::createKingsCourt() );
	}
	IF_SPELL(phase) {
		// cast the next spell three times
		game->wizard->addBuff( Buff::createPhasing() );
	}
	IF_SPELL(fury) {
		// deal double damage next turn
		game->wizard->addBuff( Buff::createFury() );
	}
	IF_SPELL(fireball) {
		// deal 10 damage
		PROJ( D(10), Color3B::RED );
	}
	IF_SPELL(mud_shield) {
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
	}
	IF_SPELL(forest_breeze) {
		// gain 5
		HEAL(5);
	} 
	IF_SPELL(lightning_bolt) {
		// deal 1-12
		PROJ( D_BETWEEN(1, 12), Color3B::YELLOW );
	}
	IF_SPELL(chill) {
		// slow enemy by 2 turns
		game->enemies[game->currentEnemy]->attack_clock += 2;
		game->hud->updateAttackClocks();
	}
	IF_SPELL(firewisp) {
		// deal 6 damage
		PROJ( D(6), Color3B::RED );
	}
	IF_SPELL(healstrike) {
		// gain 5, deal 5 damage
		HEAL(5);
		PROJ( D(5), Color3B::RED );
	}
	IF_SPELL(volcanic) {
		// deal 10 damage to everyone
		game->wizard->ui_health -= D(10);
		game->wizard->health -= D(10);
		for (Enemy *e : game->enemies) {
			e->ui_health -= D(10);
			e->health -= D(10);
		}
		game->updateHealthBars();
	}
	IF_SPELL(crystalise) {
		// heal 3, create 3 crystal gems
		HEAL(3);
		CRYSTAL(3);
	}
	IF_SPELL(zap) {
		// deal 10 to a random enemy
		PROJ_RAND( D(10), Color3B::YELLOW );
	}
	IF_SPELL(smelt) {
		// deal 6 damage, create 1 crystal gem
		PROJ( D(6), Color3B::RED );
		CRYSTAL(1);
	}
	IF_SPELL(ice_bolt) {
		// deal 3 damage, 50% chance to freeze 2
		PROJ( D(3), Color3B::BLUE );
		if (rand() % 2) {
			game->enemies[game->currentEnemy]->addBuff(
												   Buff::createFreeze(2)
												   );
		}
	}
	IF_SPELL(stun_strike) {
		// stun
		game->enemies[game->currentEnemy]->addBuff(Buff::createStun());
	}
	IF_SPELL(earthquake) {
		// 5 damage to each enemy. 50% chance to stun.
		for (Enemy *e : game->enemies) {
			e->ui_health -= D(5);
			e->health -= D(5);
			if (rand() % 2) {
				e->addBuff(Buff::createStun());
			}
		}
		game->updateHealthBars();
	} else {
		LOG("No spell definition found for %s.\n", spell->getRawName().c_str());
	}
}
