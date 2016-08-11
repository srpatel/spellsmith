//
//  GameScenery.cpp
//  Gems
//
//  Created by Sunil Patel on 18/04/2016.
//
//

#include "GameScenery.hpp"
#include "Shaders.hpp"

bool GameScenery::init(Size size) {
	if ( !Layer::init() )
	{
		return false;
	}
	
	setContentSize(size);
	
	scenery = LoadSprite("ui/scenery.png");
	scenery->setAnchorPoint(Vec2(0.5, 0.5));
	float targetWidth = size.width;
	float actualWidth = scenery->getBoundingBox().size.width;
	float ratio = targetWidth/actualWidth;
	if (ratio > 1) {
		scenery->setScale(targetWidth/actualWidth);
	}
	scenery->setPosition(Vec2(size.width/2, (size.height)/2));
	this->addChild(scenery);
	
	char_scale = size.height / scenery->getBoundingBox().size.height > 0.5 ? 1 : 0.5;
	
	redring = LoadSprite("ui/redring.png");
	redring->setAnchorPoint(Vec2(0.5, 0.2));
	redring->retain();
	
	float ydiff = size.height;
	for (int i = 0; i < 3; i++) {
		enemy_positions3[2 - i].x = size.width - 25 - char_scale * 52.5 * i;
		enemy_positions3[2 - i].y = 8 + (i % 2) * ydiff * 0.1;
	}
	
	for (int i = 0; i < 2; i++) {
		enemy_positions2[1 - i].x = size.width - 25 - char_scale * 52.5 * (i + 0.5);
		enemy_positions2[1 - i].y = 8 + (i % 2) * ydiff * 0.1;
	}
	
	for (int i = 0; i < 1; i++) {
		enemy_positions1[i].x = size.width - 25 - char_scale * 52.5 * (i + 0.5);
		enemy_positions1[i].y = 8 + (i % 2) * ydiff * 0.1;
	}
	
	wizardsprite = LoadSprite("characters/wizard.png");
	wizardsprite->setAnchorPoint(Vec2(0, 0));
	wizardsprite->setPosition(15, 0);
	wizardsprite->setScale(char_scale);
	
	this->addChild(wizardsprite);
	
	return true;
}

void GameScenery::greyscaleMode(bool grey) {
	redring->setVisible(! grey);
	GLProgram *shader = grey ? Shaders::greyscale() : Shaders::none();
	scenery->setGLProgram(shader);
	wizardsprite->setGLProgram(shader);
	if (enemies != nullptr)
		for (Enemy *e : *enemies) {
			e->sprite->setGLProgram(shader);
		}
}

void GameScenery::placeMonsters(std::vector<Enemy *> *e) {
	if (enemies != nullptr)
		for (Enemy *e : *enemies) {
			e->sprite->setGLProgram(Shaders::none());
			e->sprite->removeFromParent();
		}
	enemies = e;
	Vec2 *enemy_positions;
	if (enemies->size() == 3)
		enemy_positions = enemy_positions3;
	else if (enemies->size() == 2)
		enemy_positions = enemy_positions2;
	else
		enemy_positions = enemy_positions1;
	int i = 0;
	for (Enemy *enemy : *enemies) {
		enemy->sprite->setAnchorPoint(Vec2(0.77, 0));
		enemy->sprite->setPosition(enemy_positions[i++]);
		enemy->sprite->setScale(char_scale);
		enemy->buffHolder->setPosition(0.77 * enemy->sprite->getContentSize().width, enemy->sprite->getContentSize().height);
		addChild(enemy->sprite);
	}
	if (enemies->size() == 3) {
		(*enemies)[1]->sprite->setLocalZOrder(-1);
	} else if (enemies->size() == 2) {
		(*enemies)[0]->sprite->setLocalZOrder(-1);
	}
	redring->setScale(char_scale);
	redring->setPosition((*enemies)[0]->sprite->getPosition());
}
void GameScenery::setSelected(int selected) {
	redring->setPosition((*enemies)[selected]->sprite->getPosition());
}
GameScenery::~GameScenery() {
	redring->autorelease();
}