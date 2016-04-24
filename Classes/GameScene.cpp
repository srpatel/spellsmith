#include "GameScene.hpp"
#include "Spell.hpp"
#include "Strings.hpp"
#include "Constants.h"
#include "GameController.hpp"

#include <sstream>

#define GRID_WIDTH 4
#define GRID_HEIGHT 4

static int grid_size = 5;
Game *Game::instance = nullptr;

static struct {
	float column_height;
	float column_width;
	float bar_top_height;
	float bar_bottom_height;
	float scenery_height;
	float char_scale;
} layout;

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
	wizard->max_health = 40;
	wizard->health = 40;
	wizard->ui_health = 40;
	for (int i = 0; i < 3; i++) {
		wizard->inventory.push_back(SpellManager::get()->at(i));
	}
	
	auto scenery_sprite = LoadSprite("ui/scenery.png");
	auto right_col_sprite = LoadSprite("ui/column_right.png");
	
	// Work out layout
	float max_scenery_height = scenery_sprite->getBoundingBox().size.height;
	float min_column_height = 317;
	
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
		scenery_sprite->setAnchorPoint(Vec2(0.5, 0.5));
		// Set scale so that scenery takes up the whole width
		float targetWidth = getBoundingBox().size.width;
		float actualWidth = scenery_sprite->getBoundingBox().size.width;
		float ratio = targetWidth/actualWidth;
		if (ratio > 1) {
			scenery_sprite->setScale(targetWidth/actualWidth);
		}
		layout.scenery_height = getBoundingBox().size.height - layout.column_height;
		scenery_sprite->setPosition(Vec2(getBoundingBox().size.width/2, (getBoundingBox().size.height + layout.column_height)/2));
		this->addChild(scenery_sprite);
		
		layout.char_scale = layout.scenery_height / scenery_sprite->getBoundingBox().size.height > 0.5 ? 1 : 0.5;
		scenery = GameScenery::create(Size(getBoundingBox().size.width, getBoundingBox().size.height - layout.column_height), layout.char_scale);
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
				float halfredring = 30 * layout.char_scale;
				float startx = enemies[0]->sprite->getPositionX() - halfredring;
				float endx = enemies[enemies.size() - 1]->sprite->getPositionX() + halfredring;
				// within half red-ring size of either side
				if (p.x > startx
					&&
					p.x < endx) {
					int multiples = (int) (0.3 * layout.char_scale + (p.x - startx) / (halfredring * 2));
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
		
		currentRound = Label::createWithTTF(ToString(0), Fonts::TEXT_FONT, Fonts::TITLE_SIZE);
		currentRound->setHorizontalAlignment(TextHAlignment::CENTER);
		currentRound->setAnchorPoint(Vec2(0.5, 0.5));
		currentRound->setPosition(
			getBoundingBox().size.width - right_col_sprite->getContentSize().width/2 + 3,
			358);
		addChild(currentRound);
	}
	{
		auto grad = LayerColor::create();
		grad->initWithColor(Color4B(255, 255, 255, 255));
		grad->setPosition(0, 342);
		grad->setContentSize(Size(35, 24));
		addChild(grad);
		
		wizard_hp_bar = LayerColor::create();
		wizard_hp_bar->initWithColor(Color4B(255, 0, 0, 255));
		wizard_hp_bar->setPosition(0, 342);
		wizard_hp_bar->setContentSize(Size(35, 24));
		wizard_hp_bar->setAnchorPoint(Vec2(0, 0));
		addChild(wizard_hp_bar);
		
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
	float padding = 10;
	float margin = visibleSize.width - gridSize.x - padding;
	float starty = layout.column_height - 110;
	float spellHeight = gridSize.y/3 - padding*3;
	float spellPadding = 10;
	
	// Left hand inventory
	auto inventory = wizard->inventory;
	for (int i = 0; i < 3; i++) {
		if (inventory.size() > i) {
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
			this->addChild(sprite);
		}
	}
	// Right-hand inventory
	for (int i = 0; i < 3; i++) {
		float yoffset = (i - 1) * (spellHeight + spellPadding);
		
		if (inventory.size() > 3 + i) {
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
			sprite->setPosition(getBoundingBox().size.width - 18, grid_y - yoffset);
			_eventDispatcher->addEventListenerWithSceneGraphPriority(onSpellClick, sprite);
			// tap on a spell to see it's info. Use the "open dialog" mechanic.
			// this opens a dialog which captures events for the whole screen?
			// Dialogs::showModal(...);
			// Dialogs::closeAll();
			// TODO : Set scale that allows spell to fit completely in the scroll
			//sprite->setScale(1.f);
		//	this->addChild(sprite);
		}
	}
	
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
	float chars_y_start = layout.column_height;
	//float chars_y_end = visibleSize.height;
	// TODO : Check there is room...
	// Wizard
	auto wizardsprite = LoadSprite("characters/wizard.png");
	wizardsprite->setAnchorPoint(Vec2(0, 0));
	wizardsprite->setPosition(15, chars_y_start);
	wizardsprite->setScale(layout.char_scale);
	wizard->sprite = wizardsprite;
	this->addChild(wizardsprite);

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
	optionsButton->setPosition(layout.column_width/2 - 6, optionsButton->getContentSize().height - 3);
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

void Game::setSelected(int index) {
	currentEnemy = (index + enemies.size()) % enemies.size();
	
	int initial = currentEnemy;
	while (enemies[currentEnemy]->dead()) {
		currentEnemy = (currentEnemy + 1) % enemies.size();
		if (currentEnemy == initial) {
			// We've tried them all!
			// Shouldn't happen.
			break;
		}
	}
	
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
		if (*s == chain) {
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
				success = false;
				break;
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
			doSpell(spell);
		} else {
			// cast a chain!
			
			// immedietely apply health change to health
			// create a projectile which, when appropriate, change ui_health
			Color3B colour;
			switch (chain->type) {
				case FIRE:
					colour = Color3B::RED; break;
				case WATER:
					colour = Color3B::BLUE; break;
				case AIR:
					colour = Color3B::WHITE; break;
				case EARTH:
					colour = Color3B::GREEN; break;
				default:break;
			}
			makeProjectile(wizard, enemies[currentEnemy], damage, colour);
		}
		
		onWizardTurnOver();
	}
	return success;
}
void Game::doSpell(Spell *spell) {
	for (BaseEffect *e : spell->effects) {
		if (e->type == Projectile) {
			EffectProjectile *projectile = (EffectProjectile *) e;
			// Make a projectile!
			makeProjectile(wizard, enemies[currentEnemy], projectile->damage, Color3B::RED);
		} else if (e->type == Heal) {
			//nothing to wait for!
			int amount = ((EffectHeal *) e)->amount;
			wizard->health += amount;
			wizard->ui_health += amount;
			updateHealthBars();
		} else if (e->type == Shield) {
			Buff *shield = wizard->getBuffByType(BuffType::BARRIER);
			
			if (shield == nullptr) {
				// give us mudshield_buff, and animate it in using ga.
				auto shield = Buff::createMudshield();
				
				// put it in a good place and add it
				shield->sprite->setPosition(wizard->sprite->getPosition() + Vec2(75, 0));
				shield->sprite->setOpacity(0);
				addChild(shield->sprite);
				
				// fade it in
				auto fadeIn = FadeIn::create(0.2);
				
				addBuff(wizard, shield);
				
				shield->sprite->runAction(fadeIn);
			} else {
				shield->charges += 2;
			}
		}
	}
}
void Game::makeProjectile(Character *source, Character *target, int damage, Color3B type) {
	auto sprite = LoadSprite("spells/whiteball.png");
	sprite->setColor(type);
	float scale = 0.5 + MIN(damage, 20) / 4.f;
	sprite->setScale(scale);
	sprite->setPosition(source->sprite->getPosition().x, getContentSize().height - 100);
	sprite->setVisible(false);
	addChild(sprite);
	
	// if there's a shield, then stop early!
	Buff *shield = target->getBuffByType(BuffType::BARRIER);
	Sequence *seq;
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
		auto moveTo = MoveTo::create(1, Vec2(shield->sprite->getPosition().x, getContentSize().height - 100));
		Sprite *shieldsprite = shield->sprite;
		auto updateHealth = CallFunc::create([this, sprite, lastcharge, shieldsprite](){
			removeChild(sprite);
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
			removeBuff(target, shield);
		}
		seq = Sequence::create(Show::create(), moveTo, updateHealth, nullptr);
	} else {
		target->health -= damage;
		auto moveTo = MoveTo::create(1, Vec2(target->sprite->getPosition().x, getContentSize().height - 100));
		auto updateHealth = CallFunc::create([this, sprite, damage, target](){
			removeChild(sprite);
			target->ui_health -= damage;
			LOG("Enemy now has no health! %d v %d\n", target->ui_health, target->health);
			if (target->ui_health <= 0 && target != wizard) {
				target->sprite->removeFromParent();
			}
			updateHealthBars();
			actionDone();
		});
		seq = Sequence::create(Show::create(), moveTo, updateHealth, nullptr);
	}
	actionQueued();
	sprite->runAction(seq);
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
					// TODO
					// New level without level end dialog!
					// Should we clear buffs?
					// Should we animate going to the next level?
					// Should we wait for all current projectiles to finish?
					gotoNextEnemy();
					grid->setActive(true);
					state = kStatePlayerTurn;
				} else {
					// You are dead!
					auto fadeOut = FadeOut::create(0.2f);
					auto nextLevel = CallFunc::create([this](){
						// Dialog takes all focus!
						GameController::get()->showLevelEndDialog(false);
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
		LOG("Checking if enemies are dead\n");
		for (Enemy *e : enemies) {
			if (e->health > 0) {
				gameOver = false;
				LOG("An enemy has %d health.\n", e->health);
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
		if (! e->dead() && ! e->attack_clock--) {
			enemyTurn = true;
			// Attack!
			// Wait until your spells are done.
			runAction(Sequence::create(DelayTime::create(1), CallFunc::create([this, e]() {
				int damage = e->monster->getAttack()->amount;
				makeProjectile(e, wizard, damage, Color3B::RED);
			}), nullptr));
			
			e->attack_clock = e->monster->attack_frequency;
		}
	}
	hud->updateAttackClocks();
	
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
		delete e;
	}
	enemies.clear();
	for (Monster *m : round->monsters) {
		// create an enemy from the monster
		Enemy *enemy = new Enemy(m, enemies.size());
		enemies.push_back( enemy );
	}
	scenery->placeMonsters(&enemies);
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
		
		// Create a round based on the current stage.
		gotoNextEnemy();
		
		updateHealthBars();
		
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

void Game::addBuff(Character *target, Buff *buff) {
	// if there is already one of this type...then ignore maybe?
	// TODO
	target->buffs.push_back(buff);
	
	// Add the buff's sprite
	buff->icon->setAnchorPoint(Vec2::ZERO);
	buff->icon->setPosition(Vec2(10, target->sprite->getPosition().y));
	addChild(buff->icon);
}

void Game::removeBuff(Character *target, Buff *buff) {
	auto it = std::find(target->buffs.begin(), target->buffs.end(), buff);
	if(it != target->buffs.end()) {
		target->buffs.erase(it);
	}
	
	// Remove the buff's sprite
	removeChild(buff->icon);
	
	delete buff;
}