#include "GameScene.hpp"
#include "Spell.hpp"

#define GRID_WIDTH 5
#define GRID_HEIGHT 5

Game *Game::instance = nullptr;

Game *Game::get() {
	return instance;
}

bool Game::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
	
	instance = this;
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
	
	// Initialise spells
	// (Gems here will get created with scale 1)
	Spell::init(GRID_WIDTH, GRID_HEIGHT);
	PlayerState::get()->inventory = Spell::spells;
	
	// Backgrounds
	auto soil = Color4F(142/255.f,101/255.f,49/255.f, 255/255.f);
	auto grass = Color4F(48/255.f,114/255.f,39/255.f, 255/255.f);
	auto sky = Color4F(177/255.f,240/255.f,243/255.f, 255/255.f);
	auto background = DrawNode::create();
	background->drawSolidRect(Vec2::ZERO, Vec2(visibleSize), soil);
	background->setPosition(origin.x, origin.y);
	this->addChild(background);
	
    // Create the grid
    this->grid = new Grid(GRID_WIDTH, GRID_HEIGHT);
    cocos2d::Vec2 gridSize = this->grid->getSize();
    //this->grid->setAnchorPoint(gridSize / 2);
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + gridSize.y / 2 + 20;
    this->grid->setPosition(x, y);
    this->addChild(this->grid);
	
	// More background (must be done after grid because of sizing)
	background->drawSolidRect(Vec2(0, y + gridSize.y/2 + 20), Vec2(visibleSize.width, y + gridSize.y/2 + 60), grass);
	background->drawSolidRect(Vec2(0, y + gridSize.y/2 + 60), Vec2(visibleSize.width, visibleSize.height), sky);
	
	// Inventory
	float padding = 10;
	float margin = visibleSize.width - gridSize.x - padding;
	float spellHeight = gridSize.y/3 - padding*3;
	float spellPadding = 20;
	// If there is enough room at the sides, show spells there!
	// Otherwise, at the top?
	auto brown = Color4F(194/255.f,190/255.f,167/255.f, 255/255.f);
	// Left hand inventory
	auto inventory = PlayerState::get()->inventory;
	for (int i = 0; i < 3; i++) {
		float yoffset = (i - 1) * (spellHeight + spellPadding);
		
		auto scroll = DrawNode::create();
		scroll->drawSolidRect(Vec2(0, -spellHeight/2), Vec2(margin/2, spellHeight/2), brown);
		scroll->setPosition(origin.x, y - yoffset);
		this->addChild(scroll);
		
		if (inventory.size() > i) {
			auto sprite = inventory[i]->mininode;
			sprite->setPosition(origin.x + margin/4, y - yoffset);
			// TODO : Set scale that allows spell to fit completely in the scroll
			//sprite->setScale(1.f);
			this->addChild(sprite);
		}
	}
	
	for (int i = 0; i < 3; i++) {
		float yoffset = (i - 1) * (spellHeight + spellPadding);
		
		auto scroll = DrawNode::create();
		scroll->drawSolidRect(Vec2(0, -spellHeight/2), Vec2(margin/2, spellHeight/2), brown);
		scroll->setPosition(origin.x + visibleSize.width - margin/2, y - yoffset);
		this->addChild(scroll);
		
		if (inventory.size() > i) {
			auto sprite = inventory[3 + i]->mininode;
			sprite->setPosition(origin.x + visibleSize.width - margin/4, y - yoffset);
			// TODO : Set scale that allows spell to fit completely in the scroll
			//sprite->setScale(1.f);
			this->addChild(sprite);
		}
	}
	
    //this->scheduleUpdate();
    
    return true;
}

bool Game::onCastSpell(Chain *chain) {
	auto inventory = PlayerState::get()->inventory;
	bool success = false;
	for (Spell *s : inventory) {
		if (*s == chain) {
			success = true;
			// Spell shot
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
		while (sentinel != nullptr) {
			if (sentinel->type != t) {
				success = false;
				break;
			}
			sentinel = sentinel->next;
		}
		
		if (success) {
			// vanilla shot!
		}
	}
	return success;
}

void Game::update(float dt) {
    /*
    cocos2d::Vec2 origin = cocos2d::Director::getInstance()->getVisibleOrigin();
    cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
    cocos2d::Vec2 gridSize = this->grid->getSize();
    
    static float time = 0;
    time += dt;
    
    float x, y;
    this->grid->getPosition(&x, &y);
    x = origin.x + visibleSize.width / 2 - gridSize.x / 2 + 100 * sin(time);
    this->grid->setPosition(x, y);
     */
}

PlayerState *PlayerState::get() {
	static PlayerState *instance = nullptr;
	if (!instance) {
		instance = new PlayerState;
	}
	return instance;
}