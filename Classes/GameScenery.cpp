//
//  GameScenery.cpp
//  Gems
//
//  Created by Sunil Patel on 18/04/2016.
//
//

#include "GameScenery.hpp"

bool GameScenery::init(Size size, float scale) {
	if ( !Layer::init() )
	{
		return false;
	}
	
	char_scale = scale;
	setContentSize(size);
	
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
	
	return true;
}

void GameScenery::placeMonsters(std::vector<Enemy *> *e) {
	enemies = e;
	removeAllChildren();
	addChild(redring);
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