#include "GameScene.hpp"
#include "Spell.hpp"
#include "Strings.hpp"
#include "Constants.h"
#include "Shaders.hpp"
#include "SpellPicker.hpp"
#include "GameController.hpp"
#include "DoSpell.hpp"
#include "GameOverPopup.hpp"

#include "BasicFire.hpp"

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
	
	//layout.column_height = MAX(min_column_height, getBoundingBox().size.height - max_scenery_height);
	layout.column_height = right_col_sprite->getBoundingBox().size.height;
	layout.column_width = right_col_sprite->getBoundingBox().size.width;
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
		layout.bar_top_height = sprite->getBoundingBox().size.height;
		sprite->setAnchorPoint(Vec2(0.5, 1));
		sprite->setPosition(Vec2(getBoundingBox().size.width/2, layout.column_height));
		this->addChild(sprite);
		
		auto listener = EventListenerTouchOneByOne::create();
		listener->setSwallowTouches(true);
		listener->onTouchBegan = [sprite, this](Touch* touch, Event* event)
		{
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
		layout.bar_bottom_height = sprite->getBoundingBox().size.height;
		sprite->setAnchorPoint(Vec2(0.5, 0));
		sprite->setPosition(Vec2(getBoundingBox().size.width/2, 0));
		this->addChild(sprite);
	}
	
	// Columns
	{
		//auto sprite = LoadSprite("ui/column_right.png");
		right_col_sprite->setAnchorPoint(Vec2(1, 1));
		right_col_sprite->setPosition(Vec2(getBoundingBox().size.width, layout.column_height));
		this->addChild(right_col_sprite);
		
		auto level_counter = LoadSprite("ui/level_counter.png");
		level_counter->setAnchorPoint(Vec2(0.5, 0.5));
		level_counter->setPosition(getBoundingBox().size.width - 20, 354);
		addChild(level_counter);
		
		currentRound = Label::createWithTTF(ToString(0), Fonts::NUMBER_FONT, Fonts::SMALL_SIZE);
		currentRound->setHorizontalAlignment(TextHAlignment::CENTER);
		currentRound->setAnchorPoint(Vec2(0.5, 0.5));
		currentRound->setPosition(
			getBoundingBox().size.width - right_col_sprite->getContentSize().width/2 + 3,
			354);
		addChild(currentRound);
	}
	{
		auto grad = LoadSprite("ui/orb_bg.png");
		grad->setAnchorPoint(Vec2(0.5, 0.5));
		grad->setPosition(20, 354);
		addChild(grad);
		
		auto hp = LoadSprite("ui/orb_red.png");
		hp->setAnchorPoint(Vec2(0.5, 0));
		hp->setPosition(20, 342);
		//hp->setGLProgram(Shaders::smokey());
		addChild(hp);
		wizard_hp_bar = hp;
		
		auto sprite = LoadSprite("ui/column_left.png");
		sprite->setAnchorPoint(Vec2(0, 1));
		sprite->setPosition(Vec2(0, layout.column_height));
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
		auto sprite = LoadSprite("ui/smokey.png");
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
	//float chars_y_end = visibleSize.height;
	// TODO : Check there is room...
	// Wizard
	wizard->sprite = scenery->wizardsprite;
	wizard->buffHolder = Layer::create();
	wizard->sprite->addChild(wizard->buffHolder);
/*
 _    _ _    _ _____  
| |  | | |  | |  __ \ 
| |__| | |  | | |  | |
|  __  | |  | | |  | |
| |  | | |__| | |__| |
|_|  |_|\____/|_____/
*/
	// Hearts and options menu etc.
	
	// More background (must be done after grid because of sizing) 60 high.
	hud = GameHUD::create();
	hud->setContentSize(
		Size(getBoundingBox().size.width - layout.column_width * 2, 20 * 2));
	hud->setPosition(Vec2(layout.column_width, layout.column_height - layout.bar_top_height + 15));
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
	optionsButton->setPosition(layout.column_width/2 - 4, 34);
	addChild(optionsButton);
	
	auto onOptionsClick = EventListenerTouchOneByOne::create();
	onOptionsClick->setSwallowTouches(true);
	// trigger when you push down
	onOptionsClick->onTouchBegan = [this](Touch* touch, Event* event) -> bool {
		auto bounds = event->getCurrentTarget()->getBoundingBox();
		
		if (bounds.containsPoint(touch->getLocation())){
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
	
	const float starty = layout.column_height - 110;
	
	inventoryHolder->removeAllChildren();
	
	// Left hand inventory
	auto inventory = wizard->inventory;
	for (int i = 0; i < 3; i++) {
		if (inventory.size() > i && inventory[i] != nullptr) {
			auto sprite = inventory[i]->mininode;
			sprite->setPosition(18, starty - i * 55);
			auto onSpellClick = EventListenerTouchOneByOne::create();
			onSpellClick->setSwallowTouches(true);
			// trigger when you push down
			onSpellClick->onTouchBegan = [this, i](Touch* touch, Event* event) -> bool {
				auto bounds = event->getCurrentTarget()->getBoundingBox();
				bounds.origin -= bounds.size/2;
				
				if (bounds.containsPoint(touch->getLocation())){
					GameController::get()->showSpellInfoDialog(wizard->inventory[i]);
					return true;
				}
				
				return false; // if you are consuming it
			};
			_eventDispatcher->addEventListenerWithSceneGraphPriority(onSpellClick, sprite);
			// TODO : Set scale that allows spell to fit completely in the scroll
			sprite->setScale(1.f);
			inventoryHolder->addChild(sprite);
		}
	}
	// Right-hand inventory
	for (int i = 0; i < 3; i++) {
		if (inventory.size() > 3 + i && inventory[3+i] != nullptr) {
			auto onSpellClick = EventListenerTouchOneByOne::create();
			onSpellClick->setSwallowTouches(true);
			// trigger when you push down
			onSpellClick->onTouchBegan = [this, i](Touch* touch, Event* event) -> bool {
				auto bounds = event->getCurrentTarget()->getBoundingBox();
				bounds.origin -= bounds.size/2;
				
				if (bounds.containsPoint(touch->getLocation())){
					GameController::get()->showSpellInfoDialog(wizard->inventory[3 + i]);
					return true;
				}
				
				return false; // if you are consuming it
			};
			auto sprite = inventory[3 + i]->mininode;
			sprite->setPosition(getBoundingBox().size.width - 18, starty - i * 55);
			_eventDispatcher->addEventListenerWithSceneGraphPriority(onSpellClick, sprite);
			inventoryHolder->addChild(sprite);
		}
	}
}

int Game::getNextAliveEnemy(int start) {
	start = (start + enemies.size()) % enemies.size();
	int initial = start;
	while (enemies[start]->dead()) {
		start = (start + 1) % enemies.size();
		if (start == initial) {
			// We've tried them all!
			// Shouldn't happen.
			break;
		}
	}
	return start;
}

void Game::setSelected(int index) {
	currentEnemy = getNextAliveEnemy(index);
	
	scenery->setSelected(currentEnemy);
	hud->setSelected(currentEnemy);
}

void Game::updateHealthBars() {
	wizard_hp_bar->setScaleY(wizard->ui_health / (float) wizard->max_health);
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
	if (spell) {
			const auto size = Size(61, 88) / Director::getInstance()->getContentScaleFactor();
		
			auto glow = LayerColor::create(Color4B::WHITE, size.width, size.height);
			glow->setPosition(spell->mininode->getPosition() - size/2);
			glow->runAction(Sequence::create(
				EaseOut::create(FadeOut::create(0.5f), 0.5f),
				RemoveSelf::create(),
				nullptr)
			);
			addChild(glow);
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
			makeProjectile(wizard, enemies[currentEnemy], damage, colour);
		}
		
		onWizardTurnOver();
	}
	return success;
}
void Game::makeProjectile(Character *source, Character *target, int damage, Color3B type) {
	auto from = Vec2(source->sprite->getPosition().x + 90, getContentSize().height - 100);
	auto to = Vec2(target->sprite->getPosition().x - 60, getContentSize().height - 100);
	
	// if there's a shield, then stop early!
	Buff *shield = target->getBuffByType(BuffType::BARRIER);
	CallFunc *onHit;
	if (shield) {
		bool lastcharge = false;
		if (shield->charges > 0) {
			// Remove a charge
			shield->charges--;
			if (shield->charges == 0) {
				// Animate the shield's sprite away.
				lastcharge = true;
			}
		}
		// Don't take away any health, and just animate to the shield instead!
		//  ... and if it's the shield's last charge, fade it out too.
		to = Vec2(shield->sprite->getPosition().x, getContentSize().height - 100);
		Sprite *shieldsprite = shield->sprite;
		onHit = CallFunc::create([this, lastcharge, shieldsprite](){
			if (lastcharge) {
				// fade the shield out, then remove it.
				auto fadeout = FadeOut::create(0.2);
				auto func = CallFunc::create([this, shieldsprite](){
					removeChild(shieldsprite);
				});
				auto seq = Sequence::create(fadeout, func, nullptr);
				shieldsprite->runAction(seq);
			}
			actionDone();
		});
		
		// Remove the buff from the character
		if (lastcharge) {
			target->removeBuff(shield);
		}
	} else {
		target->health -= damage;
		onHit = CallFunc::create([this, damage, target](){
			target->ui_health -= damage;
			if (target->ui_health <= 0 && target != wizard) {
				target->sprite->removeFromParent();
			}
			updateHealthBars();
			actionDone();
		});
	}
	actionQueued();
	
	float scale = 0.5 + MIN(damage, 20) / 4.f;
	auto projectile = BasicFire::create(from, to, scale, onHit);
	addChild(projectile);
}

void Game::onWizardTurnOver() {
	// If the selected enemy has 0 hp, then you can't select it!
	setSelected(currentEnemy); // We just select the current one - the logic is in here.
	// enemy gets a shot at you!
	attemptSetState(kStateEnemySpells);
}

void Game::attemptSetState(GameState nextstate) {
	if (!checkGameOver()) {
		state = nextstate;
		if (state == kStateEnemySpells) {
			// cannot use grid until enemy done
			grid->setActive(false);
			
			// enemy casts his spell
			enemyDoTurn();
		} else {
			grid->setActive(true);
		}
	} else {
		grid->setActive(false);
		// defer all of this until all actions are done!
		PendingAction action = [this] {
			// state = gameend....
			// but for now, it's infini-mode!
			state = kStatePlayerTurn;
			
			bool success = true;
			if (wizard->health <= 0) {
				success = false;
			}
			
			std::function<void()> func;
			if (mode == kModeInfinite) {
				if (success) {
					scenery->showFlags(GameScenery::FLAG_TYPE_WIN);
					// pick a new spell if there are enough left
					if (spellpool.size() >= 2) {
						grid->setActive(false);
						// fade grid out
						auto spellPicker = SpellPicker::create(spellpool[0], spellpool[1]);
						// put it in the middle of the grid
						spellPicker->setPosition(grid->getPosition());
						addChild(spellPicker);
						// also remove another 1 or 2 randomly?
						spellpool.erase(spellpool.begin(), spellpool.begin()+2);
					}
					state = kStatePlayerTurn;
				} else {
					scenery->showFlags(GameScenery::FLAG_TYPE_LOSE);
					// You are dead!
					auto fadeOut = FadeOut::create(0.2f);
					auto nextLevel = CallFunc::create([this](){
						// Dialog takes all focus!
						//GameController::get()->showLevelEndDialog(false);
						auto levelEnd = GameOverPopup::create();
						// put it in the middle of the grid
						levelEnd->setPosition(grid->getPosition());
						addChild(levelEnd);
					});
					
					auto seq = Sequence::create(fadeOut, nextLevel, nullptr);
					wizard->sprite->runAction(seq);
				}
			} else {
				// Not planning on this :)
			}
		};
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
	if (numCurrentActions == 0) {
		for (PendingAction a : pendingActions) {
			a();
		}
		pendingActions.clear();
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
					// attack
					runAction(Sequence::create(DelayTime::create(1), CallFunc::create([this, e]() {
						int damage = e->monster->getAttack()->amount;
						makeProjectile(e, wizard, damage, Color3B::RED);
					}), nullptr));
				}
				
				e->attack_clock = e->monster->attack_frequency;
			}
		}
	}
	hud->updateAttackClocks();
	// TODO : Don't just wait 2 seconds - needs to be once things are done.
	// Use action system.
	if (!enemyTurn) {
		// it's now the player's turn
		attemptSetState(kStatePlayerTurn);
	} else {
		// Cannot use the grid until all enemies are done!
		runAction(Sequence::create(DelayTime::create(2), CallFunc::create([this]() {
			attemptSetState(kStatePlayerTurn);
		}), nullptr));
	}
}
void Game::gotoNextEnemy() {
	// we are in charge of free'ing round.
	Round *round = LevelManager::get()->generateRound(stage);
	currentRound->setString(ToString(stage));
	stage++;
	showRound(round);
	delete round;
}
void Game::showRound(Round *round) {
	// TODO : Remove all old sprites:
	for (Enemy *e : enemies) {
		e->sprite->setGLProgram(Shaders::none());
		e->sprite->removeFromParent();
		delete e;
	}
	enemies.clear();
	for (Monster *m : round->monsters) {
		// create an enemy from the monster
		Enemy *enemy = new Enemy(m, enemies.size());
		enemies.push_back( enemy );
	}
	scenery->placeMonsters(&enemies);
	scenery->showFlags(GameScenery::FLAG_TYPE_NONE);
	currentEnemy = 0;
	hud->setupMonsterList(&enemies);
	
	// reset game state
	state = kStatePlayerTurn;
	grid->setActive(true);
}
void Game::startGame(SaveGame *save) {
	if (save != nullptr) {
		// TODO
		// Load up the previous save.
		// - Current monster group (round)
		// - Look/name/hash of wizard
		// - Current score
		// - Current spells
		// - Current talents?
	} else {
		// Load a completely fresh level!
		stage = 0;
		
		// reset health
		wizard->health = wizard->max_health;
		wizard->ui_health = wizard->max_health;
		wizard->sprite->setOpacity(255);
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
		
		// Create a round based on the current stage.
		gotoNextEnemy();
		
		updateHealthBars();
		updateInventory();
		
		// shuffle grid
		grid->scramble();
		
		// reset game state
		state = kStatePlayerTurn;
		grid->setActive(true);
	}
}

void Game::update(float dt) {
	// ???
}