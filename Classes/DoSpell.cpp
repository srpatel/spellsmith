//
//  DoSpell.cpp
//  Gems
//
//  Created by Sunil Patel on 29/04/2016.
//
//

#include "DoSpell.hpp"

#define IF_SPELL(s) if (strcmp(spell->getRawName().c_str(), #s) == 0)

class AmountGenerator {
public:
	inline static int between(int min, int max) {
		return (std::rand()%(max-min)) + min;
	}
};

void DoSpell::run(Game *game, Spell *spell) {
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
			shield->sprite->setPosition(game->wizard->sprite->getPosition() + Vec2(75, 0));
			shield->sprite->setOpacity(0);
			game->addChild(shield->sprite);
			
			// fade it in
			auto fadeIn = FadeIn::create(0.2);
			
			game->addBuff(game->wizard, shield);
			
			shield->sprite->runAction(fadeIn);
		} else {
			shield->charges += 2;
		}
	} else IF_SPELL(refresh) {
		// gain 7
		game->wizard->ui_health += 7;
		game->wizard->health += 7;
		game->updateHealthBars();
	} else IF_SPELL(lightening_bolt) {
		// deal 1-6
		int amount = AmountGenerator::between(1, 6);
		game->makeProjectile(
			 game->wizard,
			 game->enemies[game->currentEnemy], amount, Color3B::RED);
	} else {
		LOG("No spell definition found for %s.\n", spell->getRawName().c_str());
	}
}