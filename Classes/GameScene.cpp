#include "GameScene.hpp"
#include "Spell.hpp"
#include "Strings.hpp"
#include "Constants.h"
#include "Dialogs.hpp"

#include <sstream>

#define GRID_WIDTH 4
#define GRID_HEIGHT 4

static int grid_size = 5;
Game *Game::instance = nullptr;

static struct {
	float column_height;
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
	Spell::init(5, 5);
	wizard = new Wizard;
	wizard->max_health = HEALTH_PER_HEART * 5;
	wizard->health = HEALTH_PER_HEART * 5;
	wizard->ui_health = HEALTH_PER_HEART * 5;
	for (int i = 0; i < 3; i++)
		wizard->inventory.push_back(Spell::spells[i]);
	
	enemy = new Enemy;
	enemy->max_health = HEALTH_PER_HEART * 2;
	enemy->health = HEALTH_PER_HEART * 2;
	enemy->ui_health = HEALTH_PER_HEART * 2;
	
	
	auto scenery_sprite = Sprite::createWithSpriteFrameName("ui/scenery.png");
	
	// Work out layout
	float scale = Director::getInstance()->getContentScaleFactor();
	float max_scenery_height = scenery_sprite->getBoundingBox().size.height;
	float min_column_height = 635 / scale;
	
	layout.column_height = MAX(min_column_height, getContentSize().height - max_scenery_height);
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
		
		scenery_sprite->setAnchorPoint(Vec2(0.5, 0));
		// Set scale so that scenery takes up the whole width
		float targetWidth = getContentSize().width;
		float actualWidth = scenery_sprite->getContentSize().width;
		float ratio = targetWidth/actualWidth;
		if (ratio > 1) {
			scenery_sprite->setScale(targetWidth/actualWidth);
		}
		scenery_sprite->setPosition(Vec2(getContentSize().width/2, layout.column_height));
		this->addChild(scenery_sprite);
	}
	
	// Columns
	{
		auto sprite = Sprite::createWithSpriteFrameName("ui/column_right.png");
		sprite->setAnchorPoint(Vec2(1, 1));
		sprite->setPosition(Vec2(getContentSize().width + 8, layout.column_height));
		this->addChild(sprite);
	}
	{
		auto sprite = Sprite::createWithSpriteFrameName("ui/column_left.png");
		sprite->setAnchorPoint(Vec2(0, 1));
		sprite->setPosition(Vec2(-8, layout.column_height));
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
    this->grid = new Grid(grid_size, grid_size, getContentSize().width - 94 * 2 / scale - 5);
    cocos2d::Vec2 gridSize = this->grid->getSize();
	float grid_x = getContentSize().width / 2;
    float grid_y = (layout.column_height) / 2;
    this->grid->setPosition(grid_x, grid_y);
	grid->active = true;
    this->addChild(this->grid);
	
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
			sprite->setPosition(17, starty - i * 55);
			// TODO : Set scale that allows spell to fit completely in the scroll
			sprite->setScale(1.f);
			this->addChild(sprite);
		}
	}
	// Right-hand inventory
	for (int i = 0; i < 3; i++) {
		float yoffset = (i - 1) * (spellHeight + spellPadding);
		
		if (inventory.size() > 3 + i) {
			auto sprite = inventory[3 + i]->mininode;
			sprite->setPosition(visibleSize.width - margin/4, grid_y - yoffset);
			// tap on a spell to see it's info. Use the "open dialog" mechanic.
			// this opens a dialog which captures events for the whole screen?
			// Dialogs::showModal(...);
			// Dialogs::closeAll();
			// TODO : Set scale that allows spell to fit completely in the scroll
			//sprite->setScale(1.f);
		//	this->addChild(sprite);
		}
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
	auto wizardsprite = Sprite::createWithSpriteFrameName("characters/wizard.png");
	wizardsprite->setAnchorPoint(Vec2(0, 0));
	wizardsprite->setPosition(10, chars_y_start);
	wizard->sprite = wizardsprite;
	this->addChild(wizardsprite);
	
	// Goblins
	auto evilwizard = Sprite::createWithSpriteFrameName("characters/goblin_01.png");
	evilwizard->setAnchorPoint(Vec2(1, 0));
	evilwizard->setPosition(this->getContentSize().width - 10, chars_y_start);
	this->addChild(evilwizard);
	enemy->sprite = evilwizard;

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
	hud->updateValues(wizard, enemy);
	hud->setPosition(Vec2(0, grid_y + gridSize.y/2 + 20));
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
	spellInfoDialog = SpellInfoDialog::create();
	spellInfoDialog->retain();
	levelEndDialog = LevelEndDialog::create();
	levelEndDialog->retain();
	
    this->scheduleUpdate();
    
    return true;
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
			LOG(s->getName().c_str());
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
			makeProjectile(wizard, enemy, damage, colour);
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
			makeProjectile(wizard, enemy, projectile->damage, Color3B::RED);
		} else if (e->type == Heal) {
			//nothing to wait for!
			int amount = ((EffectHeal *) e)->amount;
			wizard->health += amount;
			auto func = CallFunc::create([this, amount](){
				wizard->ui_health += amount;
				hud->updateValues(wizard, enemy);
			});
			func->retain();
			
			GameAnimation *ga = new GameAnimation;
			ga->target = nullptr;
			ga->action = func;
			runAnimation(ga);
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
				fadeIn->retain();
				
				addBuff(wizard, shield);
				
				GameAnimation *ga = new GameAnimation;
				ga->target = shield->sprite;
				ga->action = fadeIn;
				runAnimation(ga);
			} else {
				shield->charges += 2;
			}
		}
	}
}
void Game::makeProjectile(Character *source, Character *target, int damage, Color3B type) {
	auto sprite = Sprite::createWithSpriteFrameName("spells/whiteball.png");
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
		});
		
		// Remove the buff from the character
		if (lastcharge) {
			removeBuff(target, shield);
			delete shield;
		}
		seq = Sequence::create(Show::create(), moveTo, updateHealth, nullptr);
	} else {
		target->health -= damage;
		auto moveTo = MoveTo::create(1, Vec2(target->sprite->getPosition().x, getContentSize().height - 100));
		auto updateHealth = CallFunc::create([this, sprite, damage, target](){
			removeChild(sprite);
			target->ui_health -= damage;
			hud->updateValues(wizard, enemy);
		});
		seq = Sequence::create(Show::create(), moveTo, updateHealth, nullptr);
	}
	seq->retain();
	
	auto ga = new GameAnimation;
	ga->target = sprite;
	ga->action = seq;
	runAnimation(ga);
}
void Game::onWizardTurnOver() {
	// enemy gets a shot at you!
	attemptSetState(kStateEnemySpells);
}
void Game::attemptSetState(GameState nextstate) {
	if (!checkGameOver()) {
		state = nextstate;
		if (state == kStateEnemySpells) {
			// enemy casts his spell
			enemyDoTurn();
		}
	} else {
		grid->active = false;
		// set faded out until the next level is ready.
		grid->setOpacity(125);
		
		// state = gameend....
		// but for now, it's infini-mode!
		state = kStatePlayerTurn;
		
		// for now, reset when we die!
		if (wizard->health <= 0) {
			wizard->health = wizard->max_health;
			wizard->ui_health = wizard->max_health;
		}
		
		// if level-mode, we show the "victory screen"
		// if infini-mode, we show the next enemy -that's what we'll do in testing for now.
		// 1. fade enemy out
		// 2. reset health + fade new enemy in
		// 3. set can use grid to true!
		auto func = [this]() {
			auto fadeOut = FadeOut::create(0.2f);
			auto run1 = CallFunc::create([this]() {
				enemy->max_health = HEALTH_PER_HEART * 2;
				enemy->health = HEALTH_PER_HEART * 2;
				enemy->ui_health = HEALTH_PER_HEART * 2;
				hud->updateValues(wizard, enemy);
				//grid->scramble();
			});
			auto fadeIn = FadeIn::create(0.2f);
			auto run2 = CallFunc::create([this]() {
				grid->active = true;
			});
			auto seq = Sequence::create(fadeOut, run1, fadeIn, run2, nullptr);
			enemy->sprite->runAction(seq);
			
			levelEndDialog->setPosition(getContentSize()/2);
			addChild(levelEndDialog);
				
			state = kStatePlayerTurn;
			grid->active = true;
		};
		auto callfunc = CallFunc::create(func);
		callfunc->retain();
		
		// add this func to the queue, so it happens when there's nothing left.
		auto ga = new GameAnimation;
		ga->target = nullptr;
		ga->action = callfunc;
		runAnimation(ga);
		
	}
}
bool Game::checkGameOver() {
	// Returns if the game is over
	auto gameOver = false;
	if (wizard->health <= 0) {
		gameOver = true;
	} else if (enemy->health <= 0) {
		//need new enemy
		gameOver = true;
	}
	return gameOver;
}
void Game::enemyDoTurn() {
	int damage = 5;
	makeProjectile(enemy, wizard, damage, Color3B::RED);
	
	// it's now the player's turn
	attemptSetState(kStatePlayerTurn);
}

void Game::runAnimation(GameAnimation *ga) {
	if (currentAnimation == nullptr) {
		// no animation is currently running - so run this animation now!
		currentAnimation = ga;
		
		// add target and make it run the associated action
		auto onFinish = CallFunc::create([this, ga](){
			currentAnimation = nullptr;
			// this animation has finished, so run the next one if there is one
			if (!animation_queue.empty()) {
				runAnimation(animation_queue[0]);
				animation_queue.erase(animation_queue.begin() + 0);
			}
			ga->action->release();
			// free ga
			delete ga;
		});
		auto seq = Sequence::create(ga->action, onFinish, nullptr);
		if (ga->target) {
			ga->target->runAction(seq);
		} else {
			runAction(seq);
		}
	} else {
		// There is something running, so add this to the end of the queue
		animation_queue.push_back(ga);
	}
}

void Game::update(float dt) {
	// ???
}

bool GameHUD::init() {
	if ( !Layer::init() )
	{
		return false;
	}
	
	Size visibleSize = Director::getInstance()->getVisibleSize();
	
	// Add a couple of cheeky TBs.
	left_health = Label::createWithTTF(_("health"), Fonts::MAIN_FONT, 30);
	right_health = Label::createWithTTF(_("health"), Fonts::MAIN_FONT, 30);
	left_health->setTextColor(Color4B::WHITE);
	right_health->setTextColor(Color4B::WHITE);
	
	left_health->setPosition(Vec2(left_health->getContentSize().width/2 + 5, left_health->getContentSize().height/2));
	right_health->setPosition(Vec2(visibleSize.width - right_health->getContentSize().width/2 - 5, right_health->getContentSize().height/2));
	
	addChild(left_health);
	addChild(right_health);
	
	return true;
}

void GameHUD::updateValues(Character *left, Character *right) {
	
	Size visibleSize = Director::getInstance()->getVisibleSize();
	/*
	left_health->setString(std::to_string(left->ui_health));
	right_health->setString(std::to_string(right->ui_health));
	*/
	std::ostringstream os ;
	os << left->ui_health;
	left_health->setString(os.str());
	
	os.clear();
	os.seekp(0);
	if (right->ui_health < 10 && right->ui_health >= 0) {
		os << " ";
	}
	os << right->ui_health;
	right_health->setString(os.str());
	
	left_health->setPosition(Vec2(left_health->getContentSize().width/2 + 5, left_health->getContentSize().height/2));
	right_health->setPosition(Vec2(visibleSize.width - right_health->getContentSize().width/2 - 5, right_health->getContentSize().height/2));
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
}