#include "GameScene.hpp"
#include "Monster.hpp"
#include "Spell.hpp"
#include "Strings.hpp"
#include "Constants.h"
#include "Shaders.hpp"
#include "SpellPicker.hpp"
#include "GameController.hpp"
#include "DoSpell.hpp"
#include "GameOverPopup.hpp"
#include "SoundManager.hpp"
#include "Level.hpp"
#include "SaveData.hpp"
#include "Tutorial.hpp"
#include "Spellbook.hpp"
#include "Util.hpp"

#include "Projectiles.hpp"

#include "json/rapidjson.h"
#include "json/stringbuffer.h"
#include "json/writer.h"

#include <sstream>

#define GRID_WIDTH 4
#define GRID_HEIGHT 4

static int grid_size = 5;
Game *Game::instance = nullptr;

layout_t Game::getLayout() {
	return layout;
}

Game *Game::get() {
	return instance;
}

void Game::onSelect() {
	ColumnScreen::onSelect();
	if (post_level_dialog != nullptr) {
		post_level_dialog->removeFromParent();
		post_level_dialog->autorelease();
		post_level_dialog = nullptr;
	}
	if (game_over_dialog != nullptr) {
		game_over_dialog->removeFromParent();
		game_over_dialog->autorelease();
		game_over_dialog = nullptr;
	}
	setMapButtonVisible(true);
}

void Game::onDeselect() {
	ColumnScreen::onDeselect();
	
	// stop all sounds
	SoundManager::get()->stopHum();
	SoundManager::get()->stopPTravel();
	
	// remove all projectiles and spell animations...
	// - this can be done be clearing and resetting the scenery.
	
	// clear all pending actions
	// - not sure how this can be resolved!
}

bool Game::init() {
    if ( !ColumnScreen::init([this]() {
		// show map confirm if we don't have a post level dialog
		if (round != nullptr && round->generated) {
			saveArenaState();
			// Never show dialog if in arena.
			return false;
		}
		return post_level_dialog == nullptr && game_over_dialog == nullptr;
	}) ) {
        return false;
    }
	
	instance = this;
	
	state = kStatePlayerTurn;
	
	// Initialise spells - Normally this will be some kind of shared state.
	// (Gems here will get created with scale 1)
//	
//	scenery = GameScenery::create(Size(getBoundingBox().size.width, getBoundingBox().size.height -layout.column_height + 5), background);
//	scenery->setAnchorPoint(Vec2(0, 0));
//	scenery->setPosition(0, layout.column_height - 5);
//	addChild(scenery);
	
	{
		// Click on scenery to select the enemy at that x-pos (ish)
		auto onEnemyClick = EventListenerTouchOneByOne::create();
		onEnemyClick->setSwallowTouches(true);
		// trigger when you push down
		onEnemyClick->onTouchBegan = [this](Touch* touch, Event* event) -> bool {
			// Can't select an enemy if it's not your turn.
			if (! this->grid->isActive())
				return false;

			Vec2 p = touch->getLocation();
			Rect rect = scenery->getBoundingBox();
			if(rect.containsPoint(p)) {
				float halfredring = 30 * scenery->char_scale;
				float startx = enemies[0]->sprite->getPositionX() - halfredring;
				float endx = enemies[enemies.size() - 1]->sprite->getPositionX() + halfredring;
				// within half red-ring size of either side
				if (p.x > startx
					&&
					p.x < endx) {
					int multiples = (int) (0.3 * scenery->char_scale + (p.x - startx) / (halfredring * 2));
					if (multiples > enemies.size()) multiples = enemies.size() - 1;
					setSelected(multiples);
					return true;
				}
			}
			
			return false; // if you are consuming it
		};
		_eventDispatcher->addEventListenerWithSceneGraphPriority(onEnemyClick, this);
	}
	// Bar top
	{
		auto listener = EventListenerTouchOneByOne::create();
		listener->setSwallowTouches(true);
		listener->onTouchBegan = [this](Touch* touch, Event* event)
		{
			// Can't select an enemy if it's not your turn.
			if (! this->grid->isActive())
				return false;
			Vec2 p = touch->getLocation();
			Rect rect = bar_top->getBoundingBox();
			
			// Hard coded because we don't want to include the top part of the bar.
			if(rect.containsPoint(p) && p.y < rect.origin.y + 61)
			{
				setSelected(currentEnemy + 1);
				return true; // to indicate that we have consumed it.
			}
			
			return false;
		};
		_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, bar_top);
	}
	
	// Columns
	{
		level_counter = LoadSprite("ui/level_counter.png");
		level_counter->setAnchorPoint(Vec2(0.5, 0.5));
		level_counter->setPosition(getBoundingBox().size.width - 20 * layout.ui_scale, 354 * layout.ui_scale);
		level_counter->setScale(layout.ui_scale);
		addChild(level_counter, 6);
		
		currentRound = Label::createWithTTF(ToString(0), Fonts::NUMBER_FONT, Fonts::SMALL_SIZE);
		currentRound->setHorizontalAlignment(TextHAlignment::CENTER);
		currentRound->setAnchorPoint(Vec2(0.5, 0.5));
		currentRound->setPosition(
			getBoundingBox().size.width - layout.ui_scale * right_col_sprite->getContentSize().width/2 + layout.ui_scale * 3,
			354 * layout.ui_scale);
		addChild(currentRound, 6);
		
		currentWaveHeading = Label::createWithTTF(ToString(0), Fonts::NUMBER_FONT, Fonts::SMALLER_SIZE);
		currentWaveHeading->setString _("level.WAVE");
		currentWaveHeading->setHorizontalAlignment(TextHAlignment::CENTER);
		currentWaveHeading->setAnchorPoint(Vec2(0.5, 0.5));
		currentWaveHeading->setPosition(
			getBoundingBox().size.width - layout.ui_scale * right_col_sprite->getContentSize().width/2 + layout.ui_scale * 4,
			360 * layout.ui_scale);
		addChild(currentWaveHeading, 6);
		
		currentWave = Label::createWithTTF(ToString(0), Fonts::NUMBER_FONT, Fonts::SMALLER_SIZE);
		currentWave->setHorizontalAlignment(TextHAlignment::CENTER);
		currentWave->setAnchorPoint(Vec2(0.5, 0.5));
		currentWave->setPosition(
			getBoundingBox().size.width - layout.ui_scale * right_col_sprite->getContentSize().width/2 + layout.ui_scale * 4,
			348 * layout.ui_scale);
		addChild(currentWave, 6);
	}
	{
		auto grad = LoadSprite("ui/orb_bg.png");
		grad->setAnchorPoint(Vec2(0.5, 0.5));
		grad->setPosition(20 * layout.ui_scale, 354 * layout.ui_scale);
		grad->setScale(layout.ui_scale);
		addChild(grad, 1);
		
		auto hp = LoadSprite("ui/orb_red.png");
		hp->setAnchorPoint(Vec2(0.5, 0));
		hp->setScale(layout.ui_scale);
		hp->setPosition(20 * layout.ui_scale, 342 * layout.ui_scale);
		wizard_hp_text = Label::createWithTTF(ToString(0), Fonts::NUMBER_FONT, Fonts::SMALL_SIZE);
		wizard_hp_text->setHorizontalAlignment(TextHAlignment::CENTER);
		wizard_hp_text->setAnchorPoint(Vec2(0.5, 0.5));
		wizard_hp_text->setPosition(grad->getPosition());
		//hp->setGLProgram(Shaders::smokey());
		addChild(hp, 1);
		addChild(wizard_hp_text, 6);
		wizard_hp_bar = hp;
	}

/*
            _     _ 
           (_)   | |
  __ _ _ __ _  __| |
 / _` | '__| |/ _` |
| (_| | |  | | (_| |
 \__, |_|  |_|\__,_|
  __/ |             
 |___/              
*/
    this->grid = new Grid(grid_size, grid_size, getBoundingBox().size.width - layout.column_width * 2, layout.column_height - layout.bar_top_height - layout.bar_bottom_height);
    cocos2d::Vec2 gridSize = this->grid->getSize();
	float grid_x = getBoundingBox().size.width / 2;
    float grid_y = layout.bar_bottom_height + (layout.column_height - layout.bar_bottom_height - layout.bar_top_height) / 2;
    this->grid->setPosition(grid_x, grid_y);
	grid->setActive(true);
	
	// More background (must be done after grid because of sizing)
	//background->drawSolidRect(Vec2(0, grid_y + gridSize.y/2 + 20), Vec2(visibleSize.width, grid_y + gridSize.y/2 + 60), grass);
	//background->drawSolidRect(Vec2(0, grid_y + gridSize.y/2 + 60), Vec2(visibleSize.width, visibleSize.height), sky);

/*
 _                      _
(_)                    | |                  
 _ _ ____   _____ _ __ | |_ ___  _ __ _   _ 
| | '_ \ \ / / _ \ '_ \| __/ _ \| '__| | | |
| | | | \ V /  __/ | | | || (_) | |  | |_| |
|_|_| |_|\_/ \___|_| |_|\__\___/|_|   \__, |
                                       __/ |
                                      |___/
*/
	inventoryHolder = Layer::create();
	addChild(inventoryHolder, 6);

/*
 _    _ _    _ _____  
| |  | | |  | |  __ \ 
| |__| | |  | | |  | |
|  __  | |  | | |  | |
| |  | | |__| | |__| |
|_|  |_|\____/|_____/
*/
	// More background (must be done after grid because of sizing) 60 high.
	hud = GameHUD::create();
	hud->setContentSize(
		Size(getBoundingBox().size.width - layout.column_width * 2, layout.ui_scale * 20 * 2));
	hud->setPosition(Vec2(layout.column_width, layout.column_height - layout.bar_top_height + layout.ui_scale * 15));
	this->addChild(hud, 2); // Must be above bar_top

	// Grid must be below bar_top
	this->addChild(this->grid);
    
    return true;
}

Wizard *Game::getWizard() {
	return wizard;
}

void Game::updateInventory() {
	auto gridSize = grid->getSize();
	auto visibleSize = getContentSize();
	
	const float starty = layout.column_height - 110 * layout.ui_scale;
	
	inventoryHolder->removeAllChildren();
	for (EventListener *listener : inventoryListeners) {
		_eventDispatcher->removeEventListener(listener);
	}
	inventoryListeners.clear();

	auto inventory = wizard->inventory;
	for (int i = 0; i < 6; i++) {
		if (inventory.size() > i && inventory[i] != nullptr) {
			auto sprite = inventory[i]->mininode;
			auto y = starty - (i % 3) * 55 * layout.ui_scale;
			auto x = (i < 3) ?
				18 * layout.ui_scale :
				getBoundingBox().size.width - 18 * layout.ui_scale;
			sprite->setPosition(x, y);
			auto onSpellClick = EventListenerTouchOneByOne::create();
			onSpellClick->setSwallowTouches(true);
			// trigger when you push down
			onSpellClick->onTouchBegan = [this, i](Touch* touch, Event* event) -> bool {
				auto bounds = event->getCurrentTarget()->getBoundingBox();
				bounds.origin -= bounds.size/2;
				
				if (bounds.containsPoint(touch->getLocation())){
					PLAY_SOUND(kSoundEffect_UISelectMinor);
					GameController::get()->showSpellInfoDialog(wizard->inventory[i]);
					return true;
				}
				
				return false; // if you are consuming it
			};
			// We need to add this to a list and delete ourselves because the sprite's destructor is never called.
			// It's the shared mininode instance.
			_eventDispatcher->addEventListenerWithSceneGraphPriority(onSpellClick, sprite);
			inventoryListeners.push_back(onSpellClick);
			inventoryHolder->addChild(sprite);
		}
	}
}

int Game::getNextAliveEnemy(int start, bool *allDead) {
	start = (start + enemies.size()) % enemies.size();
	int initial = start;
	if (allDead) {
		*allDead = false;
	}
	while (enemies[start]->dead()) {
		start = (start + 1) % enemies.size();
		if (start == initial) {
			// We've tried them all!
			// Shouldn't happen.
			if (allDead) {
				*allDead = true;
			}
			break;
		}
	}
	return start;
}

bool Game::setSelected(int index) {
	bool allDead;
	currentEnemy = getNextAliveEnemy(index, &allDead);
	
	scenery->setSelected(currentEnemy);
	hud->setSelected(currentEnemy);
	
	return ! allDead;
}

void Game::updateHealthBars() {
	wizard_hp_bar->setScaleY(wizard->ui_health / (float) wizard->max_health);
	wizard_hp_text->setString(ToString(wizard->ui_health)/* + "/" + ToString(wizard->max_health)*/);
	hud->updateHealthBars();
}

bool Game::onCastSpell(Chain *chain) {
	auto inventory = wizard->inventory;
	bool success = false;
	int damage;
	Spell *spell = nullptr;
	for (Spell *s : inventory) {
		if (s != nullptr && *s == chain) {
			success = true;
			// Spell shot
			LOG("%s\n", s->getName().c_str());
			spell = s;
			Tutorial::activate(103);
			break;
		}
	}
	bool canTryChain = true;
	// We don't allow chains at some point in the tutorial
	if (Tutorial::getCurrent() == 102) {
		canTryChain = false;
	}
	// Don't allow single gems.
	// TODO : it might be possible to get locked out! Super unlikely to ever happen though...
	if (!success && chain->next != nullptr && canTryChain) {
		success = true;
		// Always allow gems all of the same colour
		auto t = chain->type;
		auto sentinel = chain->next;
		int length = 1;
		while (sentinel != nullptr) {
			if (sentinel->type != t) {
				if (t == CRYSTAL || sentinel->type == CRYSTAL) {
					// crystal always matches
					// if the first gem is crystal, the type is the first non-crystal gem.
					if (t == CRYSTAL) {
						chain->type = sentinel->type;
						t = chain->type;
					}
				} else {
					success = false;
					break;
				}
			}
			sentinel = sentinel->next;
			length++;
		}
		
		if (success) {
			// vanilla shot!
			// chain length?
			damage = length;
		}
	}
	
	if (success) {
		Tutorial::activate(104);
		// We can't draw until the enemy has had his turn
		state = kStatePlayerSpells;
		PLAY_SOUND( kSoundEffect_Cast );
		
		if (spell) {
			// cast a spell!
			DoSpell::run(this, spell, chain);
		} else {
			// cast a chain!
			
			// immedietely apply health change to health
			// create a projectile which, when appropriate, change ui_health
			ProjectileType colour;
			auto chainType = chain->type;
			if (chainType == CRYSTAL) {
				// pick one at random!
				chainType = static_cast<GemType>(1 + (rand() % 4));
			}
			switch (chainType) {
				case FIRE:  colour = ptBasicFire; break;
				case WATER: colour = ptBasicWater; break;
				case AIR:   colour = ptBasicWind; break;
				case EARTH: colour = ptBasicEarth; break;
				default:break;
			}
			
			float damageModifier = 1;
			if (wizard->getBuffByType(BuffType::FURY)) {
				damageModifier = 2;
			}
			if (wizard->getBuffByType(BuffType::FOCUS)) {
				damageModifier = 1.5;
			}
			makeProjectile(wizard, enemies[currentEnemy], (int) (damage * damageModifier), colour);
		}
		
		// Are any enemies poisoned?
		std::vector<Enemy *> poisoned;
		for (Enemy *e : enemies) {
			if (! e->dead()) {
				auto p = e->getBuffByType(BuffType::POISON);
				if (p != nullptr) {
					// Poison for 2 dams!
					e->health -= 2;
					poisoned.push_back(e);
				}
			}
		}
		// Add pending to flash them green!
		if (poisoned.size() > 0) {
			runPendingAction([this, poisoned](){
				auto delay = DelayTime::create(0.2f);
				auto flash = CallFunc::create([=](){
					for (Enemy *e : poisoned) {
						e->damageEffect(2, Colours::POISON);
						onDamageTarget(e, true);
						updateHealthBars();
					}
					actionDone();
				});
				actionQueued();
				scenery->runAction(Sequence::create(delay, flash, delay->clone(), nullptr));
			});
		}
		
		onWizardTurnOver();
	} else {
		PLAY_SOUND( kSoundEffect_Fizzle );
	}
	return success;
}
void Game::wizardBashAnimationByQueue() {
	// Do the wizard's staff bash animation, and only run queue once it hits the
	// floor.
	// Awful naming.
	runPendingAction([=]() {
		scenery->wizardsprite->setAnimation(0, "spell_aura", false);
		actionQueued();
		auto q = Sequence::create(
			DelayTime::create(0.5667f),
			CallFunc::create([=](){
				actionDone();
			}),
			nullptr
		);
		scenery->wizardsprite->runAction(q);
	});
}
void Game::makeLightning(Character *target, int amt) {
	float startx = target->sprite->getPosition().x;
	float starty = scenery->getContentSize().height - 70 * scenery->char_scale;
	target->health -= amt;
	runPendingAction([=]() {
		Vec2 pos{startx, starty};
		SoundManager::get()->playEffect(kSoundEffect_SZap);
		auto onHit = CallFunc::create([this, target, amt](){
			actionDone();
		});
		BasicAnim *anim;
		int p = rand() % 3;
		if (p == 0) {
			anim = AnimLightning1::create(pos, scenery->char_scale, onHit);
		} else if (p == 1) {
			anim = AnimLightning2::create(pos, scenery->char_scale, onHit);
		} else {
			anim = AnimLightning3::create(pos, scenery->char_scale, onHit);
		}
		anim->runAction(Sequence::create(
			DelayTime::create(0.3f),
			CallFunc::create([=](){
				target->damageEffect(amt);
				onDamageTarget(target, false);
				updateHealthBars();
			}),
			nullptr));
		scenery->addChild(anim, 100);
		actionQueued();
	});
}
void Game::makeCracks(Character *target) {
	int i = 1 + rand() % 3;
	char str[30];
	sprintf(str, "spells/cracks/cracks%02d.png", i);
	auto sprite = LoadSprite( std::string(str) );
	sprite->setScale(scenery->char_scale);
	sprite->setPosition(target->sprite->getPosition());
	sprite->runAction(Sequence::create(
		DelayTime::create(1),
		FadeOut::create(0.5),
		RemoveSelf::create(),
		nullptr
	));
	scenery->addChild(sprite);
}
void Game::makeMeteor(float xpos, float ypos, float delay) {
	scenery->runAction(Sequence::create(DelayTime::create(delay), CallFunc::create(
		[this, xpos, ypos]() {
			Vec2 to  {xpos, ypos};
			Vec2 from{xpos, ypos + getLayout().scenery_height};
			auto onHit = CallFunc::create([this](){
				actionDone();
			});
			Layer *projectile = BasicMeteor::create(from, to, scenery->char_scale, onHit);
			projectile->setRotation(90);
			scenery->addChild(projectile, 76);
		}
	), nullptr));
	actionQueued();
}
void Game::onDamageTarget(Character *target, bool withDelay) {
	spine::SkeletonAnimation *skeleton = nullptr;
	if (target->is_skeleton) {
		skeleton = (spine::SkeletonAnimation *) target->sprite;
	}
	if (target->ui_health <= 0 && target != wizard) {
		target->clearBuffs();
		if (skeleton) {
			if (withDelay) {
				actionQueued();
				auto endTime = target->die();
				// Add a second to look at the dead body
				auto delay = DelayTime::create(endTime + 0.2f);
				auto run = CallFunc::create([this](){
					actionDone();
				});
				scenery->runAction(Sequence::create(delay, run, nullptr));
			} else {
				target->die();
			}
		} else {
			target->sprite->removeFromParent();
		}
	} else if (skeleton) {
		// Run the "hit" animation!
		auto b = wizard->getBuffByType(BuffType::KINGS_COURT);
		if (target != wizard || (b == nullptr || b->turns < 2))
			skeleton->setAnimation(0, "hit", false);
	}
}
void Game::makeProjectile(Character *source, Character *target, int damage, ProjectileType type, std::function<void(void)> onhitfunc) {
	auto start_y = source->sprite->getPosition().y + source->projectile_height * scenery->char_scale;
	
	auto from = Vec2(source->sprite->getPosition().x, start_y);
	auto to = Vec2(target->sprite->getPosition().x, start_y);
	
	auto flip = (from.x > to.x) ? -1 : 1;
	
	// Add offsets
	if (source->getOffsets().count("proj_from")) {
		Vec2 o = source->getOffsets()["proj_from"];
		from.x += o.x * scenery->char_scale * flip;
		from.y += o.y * scenery->char_scale * flip;
	}
	if (target->getOffsets().count("proj_to")) {
		Vec2 o = target->getOffsets()["proj_to"];
		to.x += o.x * scenery->char_scale * flip;
		to.y += o.y * scenery->char_scale * flip;
	}
	
	// Anvil flies in from the sky
	if (type == ptBasicAnvil) {
		from = Vec2(source->sprite->getPosition().x, scenery->getContentSize().height);
	}
	
	bool phase = target->getBuffByType(BuffType::PHASING) != nullptr;
	
	// If phasing, don't take damage! And the projectile goes past?
	if (! phase) {
		target->health -= damage;
	} else {
		to.x = -50.0;
	}
	
	float scale = scenery->char_scale;//0.5 + MIN(damage, 20) / 4.f;
	if (damage >= 10) {
		scale *= 2;
	}
	
	runPendingAction([=]() {
		// Wait for 0.56 seconds as that is when the staff is in the right place
		// TODO : Events
	
		float staffTime = 14.0/30.0;
		if (source == wizard) {
			const char* projectile_type = "spell_projectile";
			if (type == ptBasicAnvil || type == ptBasicEarth) {
				projectile_type = "spell_aura";
			}
			spTrackEntry *e = scenery->wizardsprite->setAnimation(0,
				projectile_type,
				false);
			//staffTime = e->endTime - 0.6;
			// Spell aura also has a sound effect for the bash
			/*if (!strcmp(projectile_type, "spell_aura")) {
				auto delay = DelayTime::create(0.5667);
				auto run = CallFunc::create([this](){
					PLAY_SOUND(kSoundEffect_UISelect);
				});
				runAction(Sequence::create(delay, run, nullptr));
			}*/
			// Poison dart sound effect happens here instead
			if (type == ptBasicDart) {
				auto delay = DelayTime::create(0.5667);
				auto run = CallFunc::create([this](){
					PLAY_SOUND(kSoundEffect_SWhistle);
				});
				scenery->runAction(Sequence::create(delay, run, nullptr));
			}
			auto delay = DelayTime::create(e->endTime);
			auto run = CallFunc::create([this](){
				actionDone();
			});
			scenery->runAction(Sequence::create(delay, run, nullptr));
		}

		auto delay = DelayTime::create(staffTime);
		auto run = CallFunc::create([=](){
			auto onHit = CallFunc::create([this, damage, target, onhitfunc, phase, type](){
				SoundManager::get()->stopPTravel();
				if (onhitfunc) {
					onhitfunc();
				}
				if (! phase) {
					if (type == ptBasicDart) {
						target->damageEffect(damage, Colours::POISON);
					} else {
						target->damageEffect(damage);
					}
					onDamageTarget(target, true);
					updateHealthBars();
				}
				actionDone();
			});
			BasicProjectile *projectile = BasicProjectile::makeFromType(type, from, to, scale, onHit);
			if (phase) {
				projectile->turnOffSound();
			}
			scenery->addChild(projectile, 76); // in front of "back" enemies (50), and behind "front" enemies (100)
			if (type != ptBasicDart) {
				SoundManager::get()->startPTravel();
			}
		});
		scenery->runAction(Sequence::create(delay, run, nullptr));
		actionQueued(); // projectile hit
		if (source == wizard) {
			// we only wait for the wizard's animation to finish
			actionQueued(); // animation finished
		}
	});
}

void Game::onWizardTurnOver() {
	// Increment turn counter...
	numMoves++;
	// If the selected enemy has 0 hp, then you can't select it!
	setSelected(currentEnemy); // We just select the current one - the logic is in here.
	// enemy gets a shot at you!
	attemptSetState(kStateEnemySpells);
}

void Game::attemptSetState(GameState nextstate) {
	if (!checkGameOver()) {
		// Game is not over:
		state = nextstate;
		if (state == kStateEnemySpells) {
			// cannot use grid until enemy done
			grid->setActive(false);
			scenery->setRedRingVisibility(false);

			// enemy casts his spell
			enemyDoTurn();
		} else {
			// It's becoming the player's turn, so tick their buffs.
			// Tick all the wizard buffs
			// TODO - All enemy buffs too
			wizard->tickBuffs();
			scenery->setRedRingVisibility(true);

			grid->setActive(true);
			
			// If an enemy is red...
			if (enemies[0]->attack_clock == 0) {
				Tutorial::activate(5);
			}
		}
	} else {
		// Game is over!
		grid->setActive(false);
		scenery->setRedRingVisibility(false);
		// defer all of this until all actions are done!
		PendingAction action = [this] {
			// state = gameend....
			state = kStatePlayerTurn;
			
			bool success = true;
			if (wizard->health <= 0) {
				success = false;
			}
			if (success) {
				// go to the next round
				wave++;
				if (round->waves.size() == wave) {
					// There are no more waves! Show level win
					PLAY_SOUND( kSoundEffect_LevelWin );
					scenery->showFlags(GameScenery::FLAG_TYPE_WIN);
				} else {
					// There are more waves. Show the enemies.
					showRound(round, wave);
					return;
				}
			} else {
				PLAY_SOUND( kSoundEffect_LevelLose );
				scenery->showFlags(GameScenery::FLAG_TYPE_LOSE);
			}
			std::function<void()> func;
			if (round->generated) {
				// End of arena!
				if (success) {
					// Save here that all enemies are dead!
					saveArenaState();
					// pick a new spell if there are enough left
					if (spellpool.size() >= 2) {
						grid->setActive(false);
						// fade grid out
						if (post_level_dialog != nullptr) {
							post_level_dialog->removeFromParent();
							post_level_dialog->autorelease();
						}
						post_level_dialog = SpellPicker::create(spellpool[0], spellpool[1]);
						// put it in the middle of the grid
						post_level_dialog->setPosition(grid->getPosition());
						addChild(post_level_dialog, 7);
						post_level_dialog->retain();
						// also remove another 2
						spellpool.erase(spellpool.begin(), spellpool.begin() +  2);
					} else {
						// No spells left, just go for next level!
						// TODO : Looks a bit weird, add a dialog, just with a "next" button.
						spellPicked();
					}
					state = kStatePlayerTurn;
				} else {
					// You are dead!
					auto fadeOut = FadeOut::create(0.2f);
					auto nextLevel = CallFunc::create([this](){
						if (game_over_dialog != nullptr)
							game_over_dialog->removeFromParent();
						// Dialog takes all focus!
						game_over_dialog = GameOverPopup::create(false);
						game_over_dialog->retain();
						// put it in the middle of the grid
						game_over_dialog->setPosition(grid->getPosition());
						addChild(game_over_dialog);
					});
					
					auto seq = Sequence::create(fadeOut, nextLevel, nullptr);
					wizard->sprite->runAction(seq);
				}
			} else {
				if (success) {
					// Save the rewards
					// TODO : Refresh the spellbook
					bool goneToPage = false;
					for (std::string r : round->rewards) {
						bool didLearn = SaveData::addSpell(r);
						if (didLearn && ! goneToPage) {
							// Go to page with this spell
							Spellbook *sb = (Spellbook *) GameController::get()->getScreen(kStateSpellbook);
							sb->gotoPageWithSpell(r);
							goneToPage = true;
						}
					}
					
					// Save the number of moves the level was completed in
					SaveData::setLevelComplete(round->name, numMoves);
					
					// round is not generated, which means it's a normal level.
					// 1) show a dialog with the rewards, and a button to go to the spellbook.
					if (post_level_dialog != nullptr) {
						post_level_dialog->removeFromParent();
						post_level_dialog->autorelease();
					}
					
					post_level_dialog = PostLevelDialog::create(round);
					post_level_dialog->retain();
					// put it in the middle of the grid
					post_level_dialog->setPosition(grid->getPosition());
					addChild(post_level_dialog, 7);
				} else {
					// TODO : This is the same branch as above...
					
					// You are dead!
					auto nextLevel = CallFunc::create([this](){
						// Dialog takes all focus!
						if (game_over_dialog != nullptr)
							game_over_dialog->removeFromParent();
						// Dialog takes all focus!
						game_over_dialog = GameOverPopup::create(true);
						game_over_dialog->retain();
						// put it in the middle of the grid
						game_over_dialog->setPosition(grid->getPosition());
						addChild(game_over_dialog);
					});
					auto s = wizard->sprite->getBoundingBox().size;
					auto death = AnimDeath::create(
						wizard->sprite->getPosition() + Vec2{-7 * scenery->char_scale, 5 * scenery->char_scale + s.height/2},
						wizard->sprite->getScale() * 3, nextLevel);
					wizard->sprite->removeFromParent();
					scenery->addChild(death);
				}
			}
		};
		// Delay for a bit, and then do this
		runPendingAction(action);
	}
}

void Game::spellPicked() {
	gotoNextEnemy();
	grid->setActive(true);
	state = kStatePlayerTurn;
}

void Game::actionQueued() {
	numCurrentActions++;
}
void Game::actionDone() {
	numCurrentActions--;
	// only pop one and do that!
	while (numCurrentActions == 0 && pendingActions.size() > 0) {
		PendingAction a = pendingActions[0];
		a();
		pendingActions.erase(pendingActions.begin());
	}
}
void Game::runPendingAction(PendingAction a) {
	if (numCurrentActions == 0) {
		// Run it now!
		a();
	} else {
		// Run it later...
		pendingActions.push_back(a);
	}
}
bool Game::checkGameOver() {
	// Returns if the game is over
	auto gameOver = false;
	if (wizard->health <= 0) {
		gameOver = true;
	} else {
		// If every enemy dead?
		gameOver = true;
		for (Enemy *e : enemies) {
			if (e->health > 0) {
				gameOver = false;
				break;
			}
		}
	}
	return gameOver;
}
void Game::enemyDoTurn() {
	// Reduce counter on each enemy!
	bool enemyTurn = false;
	for (Enemy *e : enemies) {
		if (! e->dead()) {
			// if frozen, reduce that instead
			Buff *freeze = e->getBuffByType(BuffType::FREEZE);
			if (freeze != nullptr) {
				freeze->charges--;
				if (freeze->charges == 0) {
					// Remove the buff
					e->removeBuff(freeze);
				}
			} else if (! e->attack_clock--) {
				enemyTurn = true;
				// Attack!
				
				Buff *stun = e->getBuffByType(BuffType::STUN);
				
				if (stun != nullptr) {
					stun->charges--;
					if (stun->charges == 0) {
						// Remove the buff
						e->removeBuff(stun);
					}
				} else {
					// Should the wizard ignore damage?
					bool phase = wizard->getBuffByType(BuffType::PHASING) != nullptr;
					
					// attack
					auto a = e->monster->getAttack();
					
					std::vector<Enemy *> healtargets;
					if (a->type == kAttackTypeHeal || a->type == kAttackTypeHealOther || a->type == kAttackTypeHealSelf) {
						// Are there friendlies which need healing?
						for (Enemy *ee : enemies) {
							if (ee == e) {
								if (a->type == kAttackTypeHealOther) {
									// can't heal self if heal-other
									continue;
								}
							} else {
								if (a->type == kAttackTypeHealSelf) {
									// can't heal other if heal-self
									continue;
								}
							}
							if (ee->health < ee->max_health && ! ee->dead()) {
								healtargets.push_back(ee);
							}
						}
						if (healtargets.size() == 0) {
							// pick the attack after, regardless of ratio!
							a = e->monster->getAttackFallback( a );
						} else {
							std::random_shuffle(healtargets.begin(), healtargets.end());
						}
					}
					
					PendingAction action;
					
					if (a->type == kAttackTypeMelee) {
						// for melee attack:
						// - move up to the wizard
						// - run the specified animation
						// - move back
						
						// Deal damage immedes:
						if (! phase) {
							wizard->health -= a->amount;
						}
						
						action = [this, e, a, phase]() {
							actionQueued();
							
							int damage = a->amount;
							auto oldPos = e->sprite->getPosition();
							auto moveDistance = oldPos.x - layout.melee_spot.x;
							float moveTime = 0.5 * moveDistance / 120.0;
							
							float attackTime = 0;
							if (e->is_skeleton) {
								spine::SkeletonAnimation *skeleton = (spine::SkeletonAnimation *) e->sprite;
									
								spTrackEntry* entry = skeleton->addAnimation(0, a->animation, false, 0);
								attackTime = entry->endTime - entry->time;
							}
							
							auto moveTo = EaseOut::create(MoveTo::create(moveTime, layout.melee_spot), 0.5f);
							auto moveBack = EaseOut::create(MoveTo::create(moveTime, oldPos), 0.5f);
							auto pendingActionDone = CallFunc::create([this](){
								actionDone();
							});
							auto dealDamage = CallFunc::create([this, e, damage, phase](){
								if (! phase) {
									if (e->type == Humanoid) {
										PLAY_SOUND(kSoundEffect_Thwack);
									} else {
										PLAY_SOUND(kSoundEffect_Bite);
									}
									wizard->damageEffect(damage);
									
									auto b = wizard->getBuffByType(BuffType::KINGS_COURT);
									if (b == nullptr || b->turns < 2) {
										((spine::SkeletonAnimation *) wizard->sprite)->setAnimation(0, "hit", false);
									}
									updateHealthBars();
								}
							});
							
							auto enemySeq = Sequence::create(moveTo, DelayTime::create(attackTime), moveBack, pendingActionDone, nullptr);
							auto mainSeq = Sequence::create(DelayTime::create(moveTime + attackTime/2.0f), dealDamage, nullptr); // deal damage half way through animation
							
							e->sprite->runAction(enemySeq);
							scenery->runAction(mainSeq);
						};
					} else if (a->type == kAttackTypeHeal || a->type == kAttackTypeHealOther || a->type == kAttackTypeHealSelf) {
						// Heal -- if there is another enemy to heal, then heal.
						// Immediately heal, ui later
						Enemy *healme = healtargets[0];
						healme->health += a->amount;
						action = [this, e, a, healme]() {
							if (e->is_skeleton) {
								spine::SkeletonAnimation *skeleton = (spine::SkeletonAnimation *) e->sprite;
									
								skeleton->addAnimation(0, a->animation, false, 0);
							}
							// Wait for the heal animation
							scenery->runAction(Sequence::create(
								DelayTime::create(0.6),
								CallFunc::create([a, healme]() {
									int healfor = a->amount;
									healme->health -= healfor;
									healme->heal(healfor);
								}),
								nullptr
							));
						};
						// Otherwise, go for a melee attack instead.
					} else {
						// non-melee attacks are just basic projectile for now
						wizard->health -= a->amount;
						action = [this, e, a]() {
							if (e->is_skeleton) {
								spine::SkeletonAnimation *skeleton = (spine::SkeletonAnimation *) e->sprite;
									
								skeleton->addAnimation(0, a->animation, false, 0);
								
							}
							
							int damage = a->amount;
							wizard->health += damage;
							if (a->type == kAttackTypeProjectileFire) {
								makeProjectile(e, wizard, damage, ptBasicFire);
							} else if (a->type == kAttackTypeProjectileWater) {
								makeProjectile(e, wizard, damage, ptBasicWater);
							} else {
								makeProjectile(e, wizard, damage, ptBasicEarth);
							}
							// projectiles already add to action queue
						};
					}
					runPendingAction(action);
				}
				
				e->attack_clock = e->monster->attack_frequency;
			}
		}
	}
	hud->updateAttackClocks();
	if (round != nullptr && round->generated) {
		saveArenaState();
	}
	if (!enemyTurn) {
		// it's now the player's turn
		attemptSetState(kStatePlayerTurn);
	} else {
		// Cannot use the grid until all enemies are done!
		PendingAction action = [this]() {
			attemptSetState(kStatePlayerTurn);
		};
		runPendingAction(action);
	}
}
void Game::gotoNextEnemy() {
	// this is for arena...
	
	// Clear buffs, fully heal, and clear the grid
	wizard->clearBuffs();
	//wizard->ui_health = wizard->max_health;
	//wizard->health = wizard->max_health;
	//updateHealthBars();
	/*grid->scramble();*/
	stage++;
	// we are in charge of free'ing round.
	RoundDef *round = LevelManager::get()->generateRound(stage);
	currentRound->setString(ToString(stage));
	SaveData::setArenaScore(stage);
	showRound(round, 0);
	
	// Save state when starting a round
	saveArenaState();
}
void Game::showRound(RoundDef *round, int wave) {
	printf("Starting round. %d current actions.\n", numCurrentActions);
	setBackground(round->bg);
	if (numCurrentActions > 0) {
		numCurrentActions = 0;
	}

	if (this->round) {
		if (this->round->generated)
			delete this->round;
	}
	this->round = round;
	this->wave = wave;

	int numWaves = round->waves.size();
	currentWaveHeading->setVisible( ! round->generated && numWaves > 1 );
	currentWave->setVisible( ! round->generated && numWaves > 1 );
	auto waveXofY = Strings::get()->translate("level.WAVE_NUM",
		{
			ToString(wave + 1),
			ToString(numWaves)
		});
	currentWave->setString(waveXofY);
	
	if (numWaves > 1) {
		// Show the "wave x of y" popup
		auto sceneCentre = scenery->getPosition();
		auto sceneSize = scenery->getContentSize();
		auto waveHeading = Strings::get()->translate("level.WAVE_HEADING",
		{
			ToString(wave + 1),
			ToString(numWaves)
		});
		Layer *textPopup = makeLargeTemporaryTextPopup(waveHeading);
		auto position = sceneCentre + sceneSize/2 - textPopup->getContentSize()/2;
		textPopup->setPosition(position);
		addChild(textPopup);
	}
	
	currentRound->setVisible( round->generated );
	level_counter->setVisible( round->generated );

	// TODO : Remove all old sprites:
	for (Enemy *e : enemies) {
		e->sprite->setGLProgram(Shaders::none());
		if (wave > 0) {
			e->sprite->runAction(Sequence::create(
				FadeOut::create(0.5f),
				RemoveSelf::create(),
				nullptr
			));
		} else {
			e->sprite->removeFromParent();
		}
		delete e;
	}
	enemies.clear();
	auto monsters = round->waves[wave];
	for (std::string monster_name : monsters) {
		Monster *m = MonsterManager::get()->get(monster_name);
		printf("Showing monster %s\n", m->name.c_str());
		// create an enemy from the monster
		Enemy *enemy = new Enemy(m, enemies.size());
		enemies.push_back( enemy );
	}
	scenery->placeMonsters(&enemies, wave > 0 ? 2 : 0);
	scenery->showFlags(GameScenery::FLAG_TYPE_NONE);
	currentEnemy = 0;
	numMoves = 0;
	hud->setupMonsterList(&enemies);
	
	updateHealthBars();
	
	// reset game state
	state = kStatePlayerTurn;
	grid->setActive(true);
	
	// TUTORIAL
	// If this is round 1, and it hasn't been completed before, then we show a tutorial!
	if (round->name == "1" && wave == 0) {
		Tutorial::activate(1);
	} else if (round->name == "2" && wave == 0) {
		Tutorial::activate(101);
	}
}
void Game::saveArenaState() {
	if (wizard->health <= 0) {
		SaveData::setArenaState({});
		return;
	}
	// Arena state consists of:
	/*
	{
		"stage": 0,
		"game_over": true,
		"enemies":[
			{"name":"goblin_sword"}
		],
		"wizard":{
			"health":30,
			"inventory":["","","","","",""]
		},
		"spellpool":[
			"firewisp",
			"ice_bolt",
			...
		],
		"grid":[
			[5,5,5,5,5],
			[5,5,5,5,5],
			[5,5,5,5,5],
			[5,5,5,5,5],
			[5,5,5,5,5]
		]
	}
	*/
	
	// Save as JSON
	rapidjson::Document doc;
	doc.SetObject();
	
	auto& allocator = doc.GetAllocator();
	
	// Enemies
	rapidjson::Value theEnemies(rapidjson::kArrayType);
	for (Enemy *e : enemies) {
		rapidjson::Value enemy{};
		enemy.SetObject();
		rapidjson::Value buffs(rapidjson::kArrayType);
		for (Buff* b : e->buffs) {
			rapidjson::Value buff(rapidjson::kObjectType);
			buff.AddMember("type", b->type, allocator);
			buff.AddMember("turns", b->turns, allocator);
			buff.AddMember("charges", b->charges, allocator);
			buffs.PushBack(buff, allocator);
		}
		rapidjson::Value name;
		name.SetString(e->monster->name.c_str(), e->monster->name.length(), allocator);
		enemy.AddMember("name", name, allocator);
		enemy.AddMember("health", e->health, allocator);
		enemy.AddMember("attack_clock", e->attack_clock, allocator);
		enemy.AddMember("buffs", buffs, allocator);
		theEnemies.PushBack(enemy, allocator);
	}
	
	// Wizard
	rapidjson::Value theWizard(rapidjson::kObjectType);
	rapidjson::Value inventory(rapidjson::kArrayType);
	for (Spell * s: wizard->inventory) {
		rapidjson::Value name;
		if (s == nullptr) {
			name.SetString("");
		} else {
			name.SetString(s->getRawName().c_str(), s->getRawName().length(), allocator);
		}
		inventory.PushBack(name, allocator);
	}
	rapidjson::Value buffs(rapidjson::kArrayType);
	for (Buff* b : wizard->buffs) {
		rapidjson::Value buff(rapidjson::kObjectType);
		buff.AddMember("type", b->type, allocator);
		buff.AddMember("turns", b->turns, allocator);
		buff.AddMember("charges", b->charges, allocator);
		buffs.PushBack(buff, allocator);
	}
	theWizard.AddMember("health", wizard->health, allocator);
	theWizard.AddMember("inventory", inventory, allocator);
	theWizard.AddMember("buffs", buffs, allocator);
	
	// Spells
	// remaining spells...
	rapidjson::Value theSpellPool(rapidjson::kArrayType);
	for (Spell * s: spellpool) {
		rapidjson::Value name;
		name.SetString(s->getRawName().c_str(), s->getRawName().length(), allocator);
		theSpellPool.PushBack(name, allocator);
	}
	
	// Grid
	rapidjson::Value theGrid(rapidjson::kArrayType);
	for (int i = 0; i < grid_size; i++) {
		rapidjson::Value row(rapidjson::kArrayType);
		for (int j = 0; j < grid_size; j++) {
			auto g = grid->get(i, j);
			row.PushBack((!g) ? 0 : g->type, allocator);
		}
		theGrid.PushBack(row, allocator);
	}
	
	bool gameOver = checkGameOver();
	
	doc.AddMember("stage", stage, allocator);
	doc.AddMember("game_over", gameOver, allocator);
	doc.AddMember("enemies", theEnemies, allocator);
	doc.AddMember("wizard", theWizard, allocator);
	doc.AddMember("spellpool", theSpellPool, allocator);
	doc.AddMember("grid", theGrid, allocator);
	
	rapidjson::StringBuffer buffer{};
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	doc.Accept(writer);
	
	std::string state(buffer.GetString(), buffer.GetSize());
	printf("%s\n", state.c_str());
	SaveData::setArenaState( state );
}
static void deserialiseBuffs(rapidjson::Value &buffs, Character *c) {
	// [{...}, {...}, {...}]
	// {"type": 0, "turns": 0, "charges": 0}
	for (int i=0; i<buffs.Size(); i++) {
		auto &b = buffs[i];
		BuffType bt = static_cast<BuffType>(b["type"].GetInt());
		Buff *buff = nullptr;
		if (false);
		else if (bt == BuffType::BARRIER)
			buff = Buff::createMudshield();
		else if (bt == BuffType::FREEZE)
			buff = Buff::createFreeze(0);
		else if (bt == BuffType::STUN)
			buff = Buff::createStun();
		else if (bt == BuffType::FURY)
			buff = Buff::createFury();
		else if (bt == BuffType::KINGS_COURT)
			buff = Buff::createKingsCourt();
		else if (bt == BuffType::PHASING)
			buff = Buff::createPhasing();
		else if (bt == BuffType::FOCUS)
			buff = Buff::createFocus();
		else if (bt == BuffType::SPELL_FOCUS)
			buff = Buff::createSpellFocus();
		else if (bt == BuffType::CHARGE_BOLT)
			buff = Buff::createChargeBolt();
		else if (bt == BuffType::POISON)
			buff = Buff::createPoison();
		buff->turns = b["turns"].GetInt();
		buff->charges = b["charges"].GetInt();
		c->addBuff(buff, true);
	}
}
void Game::startArena(std::string state) {
	setup();
	
	bool createNew = true;
	
	if (! state.empty()) {
		printf("Loading: \n%s\n", state.c_str());
		// Load from the state instead!
		rapidjson::Document doc;
		if (! doc.Parse(state.c_str()).HasParseError()) {
			if (doc.IsObject()) {
				stage = doc["stage"].GetInt();
				
				auto &theWizard = doc["wizard"];
				wizard->health = theWizard["health"].GetInt();
				wizard->ui_health = wizard->health;
				wizard->inventory.clear();
				for (int i = 0; i < theWizard["inventory"].Size(); i++) {
					auto spellname = theWizard["inventory"][i].GetString();
					if (strlen(spellname) == 0) {
						wizard->inventory.push_back(nullptr);
					} else {
						auto spell = SpellManager::get()->getByName(spellname);
						wizard->inventory.push_back(spell);
					}
				}
				deserialiseBuffs(theWizard["buffs"], wizard);
				
				spellpool.clear();
				for (int i = 0; i < doc["spellpool"].Size(); i++) {
					auto spellname = doc["spellpool"][i].GetString();
					auto spell = SpellManager::get()->getByName(spellname);
					spellpool.push_back(spell);
				}
				
				// grid->scramble();
				grid->futureGems.clear();
				for (int i = 0; i < doc["grid"].Size(); i++) {
					for (int j = 0; j < doc["grid"][i].Size(); j++) {
						GemType gt = static_cast<GemType>(doc["grid"][i][j].GetInt());
						grid->futureGems.push_back(gt);
					}
				}
				grid->scramble();
				
				// enemies
				// {"name":"goblin_sword","health":20,"attack_clock":2,"buffs":[]}
				RoundDef *r = LevelManager::get()->generateRound(0);
				r->waves.clear();
				std::vector<std::string> wave;
				// If gameover, then don't add enemies!
				if (! doc["game_over"].GetBool()) {
					for (int i = 0; i < doc["enemies"].Size(); i++) {
						auto hp = doc["enemies"][i]["health"].GetInt();
						if (hp > 0) {
							wave.push_back(doc["enemies"][i]["name"].GetString());
						}
					}
				}
				r->waves.push_back(wave);
				currentRound->setString(ToString(stage));
				showRound(r, 0);
				
				if (! doc["game_over"].GetBool()) {
					for (int i = 0, j = 0; i < doc["enemies"].Size(); i++, j++) {
						auto hp = doc["enemies"][i]["health"].GetInt();
						if (hp > 0) {
							auto e = enemies[j];
							e->health = hp;
							e->ui_health = hp;
							e->attack_clock = doc["enemies"][i]["attack_clock"].GetInt();
							deserialiseBuffs(doc["enemies"][i]["buffs"], e);
						} else {
							j--;
						}
					}
				}
				updateHealthBars();
				hud->updateAttackClocks();
				
				// If enemies are all dead, show the spell picker
				attemptSetState(kStatePlayerTurn);
	
				createNew = false;
			}
		}
	}
	
	if (createNew) {
		stage = -1;
		
		wizard->inventory.clear();
		for (int i = 0; i < 6; i++) {
			wizard->inventory.push_back(nullptr);
		}
		
		// copy spells across
		spellpool = SpellManager::get()->spells;
		// sort spells by tier, but otherwise random.
		std::random_shuffle(spellpool.begin(), spellpool.end(), [](int i) { return std::rand()%i;});
		std::sort(spellpool.begin(), spellpool.end(), [](Spell *a, Spell *b) {
			return a->tier < b->tier;
		});
		// Delete the first 4 spells
		//spellpool.erase(spellpool.begin(), spellpool.begin() + 5);
		
		// shuffle grid
		grid->scramble();
		
		// Create a round based on the current stage.
		gotoNextEnemy();
	}
	
	updateInventory();
}
void Game::setup() {
	// Recreate wizard
	if (wizard != nullptr)
		delete wizard;
	wizard = new Wizard;
	wizard->max_health = 30;//1;//
	wizard->health = wizard->max_health;
	wizard->ui_health = wizard->max_health;
	wizard->projectile_height = 0.4 * 260.0 * (119.0/221.0) + 6;
	wizard->is_skeleton = true;
	updateInventory();
	
	// Recreate scenery
	if (scenery != nullptr)
		scenery->removeFromParent();
	scenery = GameScenery::create(Size(getBoundingBox().size.width, getBoundingBox().size.height -layout.column_height + 5), background);
	scenery->setAnchorPoint(Vec2(0, 0));
	scenery->setPosition(0, layout.column_height - 5);
	addChild(scenery);
	
	wizard->sprite = scenery->wizardsprite;
	wizard->buffHolder = Layer::create();
	wizard->sprite->setOpacity(255);
	wizard->sprite->addChild(wizard->buffHolder);
	
	if (SaveData::isLevelComplete("10")) {
		wizard->sprite->setSkin("crowned");
	}
	
	// TODO : Replace "100" with actual wizard width
	layout.melee_spot.x = wizard->sprite->getPositionX() + 100 * scenery->char_scale;
	layout.melee_spot.y = wizard->sprite->getPositionY() + 8 * scenery->char_scale;
	
	// clear pending actions
	numCurrentActions = 0;
	pendingActions.clear();
	
	// scramble grid
	grid->scramble();
	
	updateHealthBars();
}
ArenaScore Game::getArenaScore() {
	std::vector<std::string> inventory{};
	for (Spell *spell : wizard->inventory) {
		inventory.push_back(spell->getRawName());
	}
	return {
		.avatar = "avatar",
		.killedby = "killedby",
		.score = stage,
		.inventory = inventory
	};
}
void Game::restartRound() {
	startRound(round);
}
void Game::startRound(RoundDef *rounddef) {
	setup();
	Tutorial::cancelAll();
	
	// Set up inv
	wizard->inventory.clear();
	For (6) {
		auto sn = SaveData::getEquippedSpellAt(i);
		Spell *spell = sn.empty() ? nullptr : SpellManager::get()->getByName(sn);
		wizard->inventory.push_back(spell);
	}
	updateInventory();
	
	currentRound->setString(""); // What should go here?
	
	showRound(rounddef, 0);
}
