#include "GameScene.hpp"
#include "Spell.hpp"
#include "Strings.hpp"
#include "Constants.h"

#include <sstream>

#define GRID_WIDTH 5
#define GRID_HEIGHT 5

Game *Game::instance = nullptr;

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
	
	// Initialise spells - Normally this will be some kind of shared state.
	// (Gems here will get created with scale 1)
	Spell::init(GRID_WIDTH, GRID_HEIGHT);
	wizard = new Wizard;
	wizard->max_health = HEALTH_PER_HEART * 5;
	wizard->health = HEALTH_PER_HEART * 5;
	wizard->inventory = Spell::spells;
	
	enemy = new Enemy;
	enemy->max_health = HEALTH_PER_HEART * 2;
	enemy->health = HEALTH_PER_HEART * 2;
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
	auto soil = Color4F(142/255.f,101/255.f,49/255.f, 255/255.f);
	auto grass = Color4F(48/255.f,114/255.f,39/255.f, 255/255.f);
	auto sky = Color4F(177/255.f,240/255.f,243/255.f, 255/255.f);
	auto background = DrawNode::create();
	background->drawSolidRect(Vec2::ZERO, Vec2(visibleSize), soil);
	// The rest of the background is drawn after grid
	this->addChild(background);

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
    this->grid = new Grid(GRID_WIDTH, GRID_HEIGHT);
    cocos2d::Vec2 gridSize = this->grid->getSize();
    //this->grid->setAnchorPoint(gridSize / 2);
    float grid_x = visibleSize.width / 2;
    float grid_y = gridSize.y / 2 + 20;
    this->grid->setPosition(grid_x, grid_y);
    this->addChild(this->grid);
	
	// More background (must be done after grid because of sizing)
	background->drawSolidRect(Vec2(0, grid_y + gridSize.y/2 + 20), Vec2(visibleSize.width, grid_y + gridSize.y/2 + 60), grass);
	background->drawSolidRect(Vec2(0, grid_y + gridSize.y/2 + 60), Vec2(visibleSize.width, visibleSize.height), sky);

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
	float spellHeight = gridSize.y/3 - padding*3;
	float spellPadding = 20;
	// If there is enough room at the sides, show spells there!
	// Otherwise, at the top?
	auto brown = Color4F(194/255.f,190/255.f,167/255.f, 255/255.f);
	// Left hand inventory
	auto inventory = wizard->inventory;
	for (int i = 0; i < 3; i++) {
		float yoffset = (i - 1) * (spellHeight + spellPadding);
		
		auto scroll = DrawNode::create();
		scroll->drawSolidRect(Vec2(0, -spellHeight/2), Vec2(margin/2, spellHeight/2), brown);
		scroll->setPosition(0, grid_y - yoffset);
		this->addChild(scroll);
		
		if (inventory.size() > i) {
			auto sprite = inventory[i]->mininode;
			sprite->setPosition(margin/4, grid_y - yoffset);
			// TODO : Set scale that allows spell to fit completely in the scroll
			//sprite->setScale(1.f);
			this->addChild(sprite);
		}
	}
	// Right-hand inventory
	for (int i = 0; i < 3; i++) {
		float yoffset = (i - 1) * (spellHeight + spellPadding);
		
		auto scroll = DrawNode::create();
		scroll->drawSolidRect(Vec2(0, -spellHeight/2), Vec2(margin/2, spellHeight/2), brown);
		scroll->setPosition(visibleSize.width - margin/2, grid_y - yoffset);
		this->addChild(scroll);
		
		if (inventory.size() > i) {
			auto sprite = inventory[3 + i]->mininode;
			sprite->setPosition(visibleSize.width - margin/4, grid_y - yoffset);
			// TODO : Set scale that allows spell to fit completely in the scroll
			//sprite->setScale(1.f);
			this->addChild(sprite);
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
	float chars_y_start = grid_y + gridSize.y/2 + 60;
	//float chars_y_end = visibleSize.height;
	// TODO : Check there is room...
	// Wizard
	auto wizardsprite = Sprite::createWithSpriteFrameName("characters/wizard_body.png");
	wizardsprite->setAnchorPoint(Vec2::ZERO);
	wizardsprite->setPosition(10, chars_y_start);
	this->addChild(wizardsprite);
	
	auto armfront = Sprite::createWithSpriteFrameName("characters/wizard_arm_front.png");
	armfront->setAnchorPoint(Vec2(0.5f, 1));
	armfront->setPosition(10 + wizardsprite->getContentSize().width / 2, chars_y_start + wizardsprite->getContentSize().height / 2);
	this->addChild(armfront);
	
	// Goblins
	auto evilwizard = Sprite::createWithSpriteFrameName("characters/evil_wizard.png");
	evilwizard->setAnchorPoint(Vec2(1, 0));
	evilwizard->setPosition(this->getContentSize().width - 10, chars_y_start);
	this->addChild(evilwizard);

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
	
    //this->scheduleUpdate();
    
    return true;
}

bool Game::onCastSpell(Chain *chain) {
	auto inventory = wizard->inventory;
	bool success = false;
	for (Spell *s : inventory) {
		if (*s == chain) {
			success = true;
			// Spell shot
			LOG(s->getName().c_str());
			// Just deal 10 damage for now!
			enemy->health -= 15;
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
			enemy->health -= length;
		}
	}
	
	if (success) {
		// enemy gets a shot at you!
		auto projectile = Sprite::createWithSpriteFrameName("spells/fireball.png");
		projectile->setPosition(getContentSize().width - projectile->getContentSize().width, getContentSize().height - 100);
		addChild(projectile);
		//wizard->health -= 5;
		
		hud->updateValues(wizard, enemy);
		
		if (wizard->health <= 0) {
			// game over!
		} else if (enemy->health <= 0) {
			//need new enemy
		}
	}
	return success;
}

void Game::update(float dt) {
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
	left_health->setString(std::to_string(left->health));
	right_health->setString(std::to_string(right->health));
	*/
	std::ostringstream os ;
	os << left->health;
	left_health->setString(os.str());
	
	os.clear();
	os.seekp(0);
	os << right->health;
	right_health->setString(os.str());
	
	left_health->setPosition(Vec2(left_health->getContentSize().width/2 + 5, left_health->getContentSize().height/2));
	right_health->setPosition(Vec2(visibleSize.width - right_health->getContentSize().width/2 - 5, right_health->getContentSize().height/2));
}