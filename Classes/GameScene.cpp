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

#include "Projectiles.hpp"

#include <sstream>

#define GRID_WIDTH 4
#define GRID_HEIGHT 4

static int grid_size = 5;
Game *Game::instance = nullptr;

static layout_t layout;

layout_t Game::getLayout() {
	return layout;
}

Game *Game::get() {
	return instance;
}

bool Game::init() {
    if ( !Layer::init() ) {
        return false;
    }
	
	instance = this;
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
	setContentSize(visibleSize);
	
	state = kStatePlayerTurn;
	
	// Initialise spells - Normally this will be some kind of shared state.
	// (Gems here will get created with scale 1)
	wizard = new Wizard;
	wizard->max_health = 30;//1;//
	wizard->health = wizard->max_health;
	wizard->ui_health = wizard->max_health;
	
	auto right_col_sprite = LoadSprite("ui/column_right.png");
	
	
	layout.column_height = right_col_sprite->getBoundingBox().size.height;
	layout.column_width = right_col_sprite->getBoundingBox().size.width;
	
	// if the scenery is too small...
	float min_scenery_height = 105;
	float ui_scale = 1;
	float current_scenery_height = getBoundingBox().size.height - layout.column_height;
	if (current_scenery_height < min_scenery_height) {
		layout.column_height = getBoundingBox().size.height - min_scenery_height;
		ui_scale = layout.column_height / right_col_sprite->getBoundingBox().size.height;
		layout.column_width *= ui_scale;
	}
	layout.ui_scale = ui_scale;
/*
 _                _                                   _     
| |              | |                                 | |    
| |__   __ _  ___| | ____ _ _ __ ___  _   _ _ __   __| |___ 
| '_ \ / _` |/ __| |/ / _` | '__/ _ \| | | | '_ \ / _` / __|
| |_) | (_| | (__|   < (_| | | | (_) | |_| | | | | (_| \__ \
|_.__/ \__,_|\___|_|\_\__, |_|  \___/ \__,_|_| |_|\__,_|___/
                       __/ |                                
                      |___/
*/
	// Scenery
	{
		layout.scenery_height = getBoundingBox().size.height - layout.column_height;
		
		scenery = GameScenery::create(Size(getBoundingBox().size.width, getBoundingBox().size.height - layout.column_height));
		scenery->setAnchorPoint(Vec2(0, 0));
		scenery->setPosition(0, layout.column_height);
		addChild(scenery);
		
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
		_eventDispatcher->addEventListenerWithSceneGraphPriority(onEnemyClick, scenery);
	}
	// Gem background
	{
		auto grad = LayerColor::create();
		grad->initWithColor(Color4B(Colours::GEM_BACKGROUND));
		grad->setPosition(Director::getInstance()->getVisibleOrigin());
		grad->setContentSize(Size(getBoundingBox().size.width, layout.column_height));
		this->addChild(grad);
	}
	// Bar top
	{
		auto sprite = LoadSprite("ui/bar_top.png");
		layout.bar_top_height = sprite->getBoundingBox().size.height * ui_scale;
		sprite->setAnchorPoint(Vec2(0.5, 1));
		sprite->setPosition(Vec2(getBoundingBox().size.width/2, layout.column_height));
		sprite->setScale(ui_scale);
		this->addChild(sprite);
		
		auto listener = EventListenerTouchOneByOne::create();
		listener->setSwallowTouches(true);
		listener->onTouchBegan = [sprite, this](Touch* touch, Event* event)
		{
			// Can't select an enemy if it's not your turn.
			if (! this->grid->isActive())
				return false;
			Vec2 p = touch->getLocation();
			Rect rect = sprite->getBoundingBox();
			
			// Hard coded because we don't want to include the top part of the bar.
			if(rect.containsPoint(p) && p.y < rect.origin.y + 61)
			{
				setSelected(currentEnemy + 1);
				return true; // to indicate that we have consumed it.
			}
			
			return false;
		};
		_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, sprite);
	}
	
	// bar bottom
	{
		auto sprite = LoadSprite("ui/bar_bottom.png");
		layout.bar_bottom_height = sprite->getBoundingBox().size.height * ui_scale;
		sprite->setAnchorPoint(Vec2(0.5, 0));
		sprite->setPosition(Vec2(getBoundingBox().size.width/2, 0));
		sprite->setScale(ui_scale);
		this->addChild(sprite);
	}
	
	// Columns
	{
		//auto sprite = LoadSprite("ui/column_right.png");
		right_col_sprite->setAnchorPoint(Vec2(1, 1));
		right_col_sprite->setPosition(Vec2(getBoundingBox().size.width, layout.column_height));
		right_col_sprite->setScale(ui_scale);
		this->addChild(right_col_sprite);
		
		level_counter = LoadSprite("ui/level_counter.png");
		level_counter->setAnchorPoint(Vec2(0.5, 0.5));
		level_counter->setPosition(getBoundingBox().size.width - 20 * ui_scale, 354 * ui_scale);
		level_counter->setScale(ui_scale);
		addChild(level_counter);
		
		currentRound = Label::createWithTTF(ToString(0), Fonts::NUMBER_FONT, Fonts::SMALL_SIZE);
		currentRound->setHorizontalAlignment(TextHAlignment::CENTER);
		currentRound->setAnchorPoint(Vec2(0.5, 0.5));
		currentRound->setPosition(
			getBoundingBox().size.width - ui_scale * right_col_sprite->getContentSize().width/2 + ui_scale * 3,
			354 * ui_scale);
		addChild(currentRound);
	}
	{
		auto grad = LoadSprite("ui/orb_bg.png");
		grad->setAnchorPoint(Vec2(0.5, 0.5));
		grad->setPosition(20 * ui_scale, 354 * ui_scale);
		grad->setScale(ui_scale);
		addChild(grad);
		
		auto hp = LoadSprite("ui/orb_red.png");
		hp->setAnchorPoint(Vec2(0.5, 0));
		hp->setScale(ui_scale);
		hp->setPosition(20 * ui_scale, 342 * ui_scale);
		wizard_hp_text = Label::createWithTTF(ToString(0), Fonts::NUMBER_FONT, Fonts::SMALL_SIZE);
		wizard_hp_text->setHorizontalAlignment(TextHAlignment::CENTER);
		wizard_hp_text->setAnchorPoint(Vec2(0.5, 0.5));
		wizard_hp_text->setPosition(grad->getPosition());
		//hp->setGLProgram(Shaders::smokey());
		addChild(hp);
		addChild(wizard_hp_text);
		wizard_hp_bar = hp;
		
		auto sprite = LoadSprite("ui/column_left.png");
		sprite->setAnchorPoint(Vec2(0, 1));
		sprite->setPosition(Vec2(0, layout.column_height));
		sprite->setScale(ui_scale);
		this->addChild(sprite);
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
	addChild(inventoryHolder);
	updateInventory();
	
	// Smokey
	{
		auto sprite = LoadLargeSprite("smokey.png");
		sprite->setAnchorPoint(Vec2(0.5, 1));
		sprite->setPosition(Vec2(getBoundingBox().size.width/2, layout.column_height));
		this->addChild(sprite);
	}
	
/*
      _                          _                
     | |                        | |               
  ___| |__   __ _ _ __ __ _  ___| |_ ___ _ __ ___ 
 / __| '_ \ / _` | '__/ _` |/ __| __/ _ \ '__/ __|
| (__| | | | (_| | | | (_| | (__| ||  __/ |  \__ \
 \___|_| |_|\__,_|_|  \__,_|\___|\__\___|_|  |___/
*/
	// Wizard
	wizard->sprite = scenery->wizardsprite;
	// TODO : calculate height from skeleton bounding box?
	wizard->projectile_height = 0.4 * 260.0 * (119.0/221.0) + 6;
	wizard->is_skeleton = true;
	wizard->buffHolder = Layer::create();
	wizard->sprite->addChild(wizard->buffHolder);
	// TODO : Replace "100" with actual wizard width
	layout.melee_spot.x = wizard->sprite->getPositionX() + 100 * scenery->char_scale;
	layout.melee_spot.y = wizard->sprite->getPositionY() + 8 * scenery->char_scale;
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
	this->addChild(hud);
	
/*              _ _                  _ _
               | | |                (_) |           
 ___ _ __   ___| | |  ___ _ __  _ __ _| |_ ___  ___ 
/ __| '_ \ / _ \ | | / __| '_ \| '__| | __/ _ \/ __|
\__ \ |_) |  __/ | | \__ \ |_) | |  | | ||  __/\__ \
|___/ .__/ \___|_|_| |___/ .__/|_|  |_|\__\___||___/
    | |                  | |                        
    |_|                  |_|
*/

	

/*   _ _       _
    | (_)     | |                
  __| |_  __ _| | ___   __ _ ___ 
 / _` | |/ _` | |/ _ \ / _` / __|
| (_| | | (_| | | (_) | (_| \__ \
 \__,_|_|\__,_|_|\___/ \__, |___/
                        __/ |    
                       |___/
 */
	Sprite *optionsButton = LoadSprite("ui/options.png");
	optionsButton->setAnchorPoint(Vec2(0.5, 0.5));
	optionsButton->setScale(ui_scale);
	optionsButton->setPosition(layout.column_width/2 - 4, 34 * ui_scale);
	addChild(optionsButton);
	
	auto onOptionsClick = EventListenerTouchOneByOne::create();
	onOptionsClick->setSwallowTouches(true);
	// trigger when you push down
	onOptionsClick->onTouchBegan = [this](Touch* touch, Event* event) -> bool {
		auto bounds = event->getCurrentTarget()->getBoundingBox();
		
		if (bounds.containsPoint(touch->getLocation())) {
			PLAY_SOUND(kSoundEffect_UISelectMinor);
			GameController::get()->showOptionsDialog();
			return true;
		}
		
		return false; // if you are consuming it
	};
	_eventDispatcher->addEventListenerWithSceneGraphPriority(onOptionsClick, optionsButton);
	
	// Grid must be topmost.
	this->addChild(this->grid);
	
    this->scheduleUpdate();
    
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
			break;
		}
	}
	// Don't allow single gems.
	// TODO : it might be possible to get locked out! Super unlikely to ever happen though...
	if (!success && chain->next != nullptr) {
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
			Color3B colour;
			switch (chain->type) {
				case FIRE:  colour = Color3B::RED; break;
				case WATER: colour = Color3B::BLUE; break;
				case AIR:   colour = Color3B::WHITE; break;
				case EARTH: colour = Color3B::GREEN; break;
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
		
		onWizardTurnOver();
	} else {
		PLAY_SOUND( kSoundEffect_Fizzle );
	}
	return success;
}
void Game::makeProjectile(Character *source, Character *target, int damage, Color3B type, std::function<void(void)> onhitfunc) {
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
	
	bool phase = target->getBuffByType(BuffType::PHASING) != nullptr;
	CallFunc *onHit;
	
	// TODO -- if phasing, don't take damage! And the projectile goes past?
	if (! phase) {
		target->health -= damage;
	}
	onHit = CallFunc::create([this, damage, target, onhitfunc, phase](){
		SoundManager::get()->stopPTravel();
		if (onhitfunc) {
			onhitfunc();
		}
		spine::SkeletonAnimation *skeleton = nullptr;
		if (target->is_skeleton) {
			skeleton = (spine::SkeletonAnimation *) target->sprite;
		}
		if (! phase) {
			target->damageEffect(damage);
			if (target->ui_health <= 0 && target != wizard) {
				if (skeleton) {
					actionQueued();
					auto endTime = target->die();
					// Add a second to look at the dead body
					auto delay = DelayTime::create(endTime + 0.2f);
					auto run = CallFunc::create([this](){
						actionDone();
					});
					runAction(Sequence::create(delay, run, nullptr));
				} else {
					target->sprite->removeFromParent();
				}
			} else if (skeleton) {
				// Run the "hit" animation!
				skeleton->addAnimation(0, "hit", false);
			}
			updateHealthBars();
		}
		actionDone();
	});
	
	float scale = scenery->char_scale;//0.5 + MIN(damage, 20) / 4.f;
	Layer *projectile;
	if (type == Color3B::WHITE) {
		projectile = BasicWind::create(from, to, scale, onHit);
	} else if (type == Color3B::GREEN) {
		projectile = BasicEarth::create(from, to, scale, onHit);
	} else if (type == Color3B::BLUE) {
		projectile = BasicWater::create(from, to, scale, onHit);
	} else {
		projectile = BasicFire::create(from, to, scale, onHit);
	}
	
	// Wait for 0.56 seconds as that is when the staff is in the right place
	// TODO : Events
	projectile->retain();
	
	runPendingAction([this, source, projectile, type]() {
		if (source == wizard) {
			spTrackEntry *e = scenery->wizardsprite->addAnimation(0,
				type == Color3B::GREEN ? "spell_aura" : "spell_projectile",
				false);
			// Spell aura also has a sound effect for the bash
			/*if (type == Color3B::GREEN) {
				auto delay = DelayTime::create(0.5667);
				auto run = CallFunc::create([this](){
					PLAY_SOUND(kSoundEffect_UISelect);
				});
				runAction(Sequence::create(delay, run, nullptr));
			}*/
			auto delay = DelayTime::create(e->endTime);
			auto run = CallFunc::create([this](){
				actionDone();
			});
			runAction(Sequence::create(delay, run, nullptr));
		}
		auto delay = DelayTime::create(14.0/30.0);
		auto run = CallFunc::create([this, projectile](){
			scenery->addChild(projectile, 76); // in front of "back" enemies (50), and behind "front" enemies (100)
			SoundManager::get()->startPTravel();
			projectile->release();
		});
		runAction(Sequence::create(delay, run, nullptr));
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
		}
	} else {
		// Game is over!
		grid->setActive(false);
		// defer all of this until all actions are done!
		PendingAction action = [this] {
			// state = gameend....
			state = kStatePlayerTurn;
			
			bool success = true;
			if (wizard->health <= 0) {
				success = false;
			}
			if (success) {
				PLAY_SOUND( kSoundEffect_LevelWin );
				scenery->showFlags(GameScenery::FLAG_TYPE_WIN);
			} else {
				PLAY_SOUND( kSoundEffect_LevelLose );
				scenery->showFlags(GameScenery::FLAG_TYPE_LOSE);
			}
			std::function<void()> func;
			if (round->generated) {
				if (success) {
					// pick a new spell if there are enough left
					if (spellpool.size() >= 2) {
						grid->setActive(false);
						// fade grid out
						auto spellPicker = SpellPicker::create(spellpool[0], spellpool[1]);
						// put it in the middle of the grid
						spellPicker->setPosition(grid->getPosition());
						addChild(spellPicker);
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
						// Dialog takes all focus!
						auto levelEnd = GameOverPopup::create();
						// put it in the middle of the grid
						levelEnd->setPosition(grid->getPosition());
						addChild(levelEnd);
					});
					
					auto seq = Sequence::create(fadeOut, nextLevel, nullptr);
					wizard->sprite->runAction(seq);
				}
			} else {
				if (success) {
					// Save the rewards
					// TODO : Refresh the spellbook
					for (std::string r : round->rewards) {
						SaveData::addSpell(r);
					}
					
					// Save the number of moves the level was completed in
					SaveData::setLevelComplete(round->name, numMoves);
					
					// round is not generated, which means it's a normal level.
					// 1) show a dialog with the rewards, and a button to go to the spellbook.					
					auto dialog = PostLevelDialog::create(round);
					// put it in the middle of the grid
					dialog->setPosition(grid->getPosition());
					addChild(dialog);
				} else {
					// TODO : This is the same branch as above...
					
					// You are dead!
					auto fadeOut = FadeOut::create(0.2f);
					auto nextLevel = CallFunc::create([this](){
						// Dialog takes all focus!
						auto levelEnd = GameOverPopup::create();
						// put it in the middle of the grid
						levelEnd->setPosition(grid->getPosition());
						addChild(levelEnd);
					});
					
					auto seq = Sequence::create(fadeOut, nextLevel, nullptr);
					wizard->sprite->runAction(seq);
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
				
				// TODO : Not 1 second - actual length of character attack
				if (stun != nullptr) {
					stun->charges--;
					if (stun->charges == 0) {
						// Remove the buff
						e->removeBuff(stun);
					}
					// Get up instead of attacking
					runAction(Sequence::create(DelayTime::create(1), CallFunc::create([this, e]() {
						// Get up!
					}), nullptr));
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
							printf("Falling back!\n");
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
							if (! phase) {
								wizard->health -= damage;
							}
							auto dealDamage = CallFunc::create([this, e, damage, phase](){
								if (! phase) {
									if (e->type == Humanoid) {
										PLAY_SOUND(kSoundEffect_Thwack);
									} else {
										PLAY_SOUND(kSoundEffect_Bite);
									}
									wizard->damageEffect(damage);
									((spine::SkeletonAnimation *) wizard->sprite)->addAnimation(0, "hit", false);
									updateHealthBars();
								}
							});
							
							auto enemySeq = Sequence::create(moveTo, DelayTime::create(attackTime), moveBack, pendingActionDone, nullptr);
							auto mainSeq = Sequence::create(DelayTime::create(moveTime + attackTime/2.0f), dealDamage, nullptr); // deal damage half way through animation
							
							e->sprite->runAction(enemySeq);
							runAction(mainSeq);
						};
					} else if (a->type == kAttackTypeHeal || a->type == kAttackTypeHealOther || a->type == kAttackTypeHealSelf) {
						// Heal -- if there is another enemy to heal, then heal.
						action = [this, e, a, healtargets]() {
							if (e->is_skeleton) {
								spine::SkeletonAnimation *skeleton = (spine::SkeletonAnimation *) e->sprite;
									
								skeleton->addAnimation(0, a->animation, false, 0);
							}
							// Wait for the heal animation
							runAction(Sequence::create(
								DelayTime::create(0.6),
								CallFunc::create([a, healtargets]() {
									int healfor = a->amount;
							
									Enemy *healme = healtargets[0];
									healme->heal(healfor);
								}),
								nullptr
							));
						};
						// Otherwise, go for a melee attack instead.
					} else {
						// non-melee attacks are just basic projectile for now
						action = [this, e, a]() {
							if (e->is_skeleton) {
								spine::SkeletonAnimation *skeleton = (spine::SkeletonAnimation *) e->sprite;
									
								skeleton->addAnimation(0, a->animation, false, 0);
								
							}
							
							int damage = a->amount;
							if (a->type == kAttackTypeProjectileFire) {
								makeProjectile(e, wizard, damage, Color3B::RED);
							} else if (a->type == kAttackTypeProjectileWater) {
								makeProjectile(e, wizard, damage, Color3B::BLUE);
							} else {
								makeProjectile(e, wizard, damage, Color3B::GREEN);
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
	// Clear buffs, fully heal, and clear the grid
	wizard->clearBuffs();
	wizard->ui_health = wizard->max_health;
	wizard->health = wizard->max_health;
	updateHealthBars();
	/*grid->scramble();*/
	
	// we are in charge of free'ing round.
	RoundDef *round = LevelManager::get()->generateRound(stage);
	currentRound->setString(ToString(stage));
	stage++;
	showRound(round);
}
void Game::showRound(RoundDef *round) {
	if (this->round) {
		if (this->round->generated)
			delete this->round;
	}
	this->round = round;

	currentRound->setVisible( round->generated );
	level_counter->setVisible( round->generated );

	// TODO : Remove all old sprites:
	for (Enemy *e : enemies) {
		e->sprite->setGLProgram(Shaders::none());
		e->sprite->removeFromParent();
		delete e;
	}
	enemies.clear();
	for (std::string monster_name : round->monsters) {
		Monster *m = MonsterManager::get()->get(monster_name);
		printf("Showing monster %s\n", m->name.c_str());
		// create an enemy from the monster
		Enemy *enemy = new Enemy(m, enemies.size());
		enemies.push_back( enemy );
	}
	scenery->placeMonsters(&enemies);
	scenery->showFlags(GameScenery::FLAG_TYPE_NONE);
	currentEnemy = 0;
	numMoves = 0;
	hud->setupMonsterList(&enemies);
	
	updateHealthBars();
	
	// reset game state
	state = kStatePlayerTurn;
	grid->setActive(true);
}
void Game::startArena() {
	// Load a completely fresh level!
	stage = 0;
	
	setup();
	
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
	
	// Create a round based on the current stage.
	gotoNextEnemy();
	
	updateInventory();
	
	// shuffle grid
	grid->scramble();
	
	// reset game state
	state = kStatePlayerTurn;
	grid->setActive(true);
}
void Game::setup() {
	// reset health
	wizard->health = wizard->max_health;
	wizard->ui_health = wizard->max_health;
	wizard->sprite->setOpacity(255);
	
	updateHealthBars();
}
void Game::startRound(RoundDef *rounddef) {
	setup();
	
	// Set up inv
	wizard->inventory.clear();
	For (6) {
		auto sn = SaveData::getEquippedSpellAt(i);
		Spell *spell = sn.empty() ? nullptr : SpellManager::get()->getByName(sn);
		wizard->inventory.push_back(spell);
	}
	updateInventory();
	
	currentRound->setString(""); // What should go here?
	showRound(rounddef);
}

void Game::update(float dt) {
	// ???
}
