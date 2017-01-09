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
#define PROJ(_n_, _t_) projectile = true; \
	game->makeProjectile(\
	game->wizard, \
	game->enemies[game->currentEnemy], \
	_n_,\
	_t_);
#define PROJ_RAND(_n_, _t_) projectile = true; \
	{ int i = game->getNextAliveEnemy(rand() % game->enemies.size(), nullptr);\
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
	// King's court is at the bottom
	// because we want to queue pending actions after any pending actions here are added.
	bool projectile = false;
	bool hasKingsCourt = game->wizard->getBuffByType(BuffType::KINGS_COURT);
	
	if (1 == 0);
	IF_SPELL(whispers_in_the_wind) {
		// Destroy all your air gems, and cast three of your spells randomly.
		// Repeat the spell twice more!
		std::vector<Spell *> spells;
		for (Spell *s : game->wizard->inventory) {
			if (s != nullptr && s != spell) {
				spells.push_back(s);
			}
		}
		std::random_shuffle(spells.begin(), spells.end(), [](int i) { return std::rand()%i;});
		for (int i = 0; i < 3; i++) {
			if (spells.size() > i) {
				PendingAction action = [game, spells, i, chain] {
					if (game->setSelected(game->currentEnemy)) {
						game->actionQueued();
						run(game, spells[i], chain, false);
						game->actionDone();
					}
				};
				game->runPendingAction(action);
			}
		}
		game->grid->destroyGemsOfType( GemType::AIR, chain );
	}
	IF_SPELL(induce_explosion) {
		// Deal 5 damage. If this kills the enemy, deal 10 damage to all other enemies.
		auto target = game->enemies[game->currentEnemy];
		PROJ( D(5), Color3B::RED );
		// Should occur when the proj hits!
		if (target->health <= 0) {
			// Deal 10 the others too!
			for (Enemy *e : game->enemies) {
				if (e == target) continue;
				e->ui_health -= D(10);
				e->health -= D(10);
			}
			// update when the proj hits!
			//game->updateHealthBars();
		}
	}
	IF_SPELL(poison_dart) {
		// Deal 5 damage. Deal an extra 10 damage if they are below 50% health.
		int n = 5;
		auto e = game->enemies[game->currentEnemy];
		if (e->health * 2 < e->max_health) {
			n += 10;
		}
		PROJ( D(n), Color3B::GREEN );
	}
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
	}
	IF_SPELL(dragon_breath) {
		// Deal 7 damage to each enemy.
		for (Enemy *e : game->enemies) {
			e->ui_health -= D(7);
			e->health -= D(7);
		}
		game->updateHealthBars();
	}
	IF_SPELL(lightning_storm) {
		// Deal 3-10 damage to each enemy.
		for (Enemy *e : game->enemies) {
			int amt = D_BETWEEN(3, 10);
			e->ui_health -= amt;
			e->health -= amt;
		}
		game->updateHealthBars();
	}
	else {
		LOG("No spell definition found for %s.\n", spell->getRawName().c_str());
	}
	
	if (! projectile) {
		game->scenery->wizardsprite->addAnimation(0, "spell_aura", false); // aura spell
	} else {
		game->scenery->wizardsprite->addAnimation(0, "spell_projectile", false); // aura spell
	}
	
	// Flash the inventory icon
	const auto size = Size(30 * game->getLayout().ui_scale, 44 * game->getLayout().ui_scale);
	auto glow = LayerColor::create(Color4B::WHITE, size.width, size.height);
	glow->setPosition(spell->mininode->getPosition() - size/2);
	glow->runAction(Sequence::create(
		EaseOut::create(FadeOut::create(0.5f), 0.5f),
		RemoveSelf::create(),
		nullptr)
	);
	game->addChild(glow);

	// King's Court
	if (allowRepeats && hasKingsCourt) {
		// Repeat the spell twice more!
		PendingAction action = [game, spell, chain] {
			if (game->setSelected(game->currentEnemy)) {
				game->actionQueued();
				run(game, spell, chain, false);
				game->actionDone();
			}
		};
		game->runPendingAction(action);
		game->runPendingAction(action);
	}
}
