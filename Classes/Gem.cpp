//
//  Gem.cpp
//  GemGame
//
//  Created by Sunil Patel on 21/10/2015.
//
//

#include "Gem.hpp"

float Gem::scale = 1;

void Gem::init(GemType type) {
	didInit = true;
	const char *element;
	switch (type) {
		case AIR: element = "gems/air_01.png"; break;
		case EARTH: element = "gems/earth_01.png"; break;
		case FIRE: element = "gems/fire_01.png"; break;
		case WATER: element = "gems/water_01.png"; break;
		case NONE: /*hopefully won't happen!*/ break;
	}
	this->type = type;
	
	sprite = Sprite::createWithSpriteFrameName(element);
	sprite->setScale(Gem::scale);
}

void Gem::init() {
    init(static_cast<GemType>(1 + rand() % 4));
}

Size Gem::getSize() {
	auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName("gems/air_01.png");
	return frame->getRect().size * scale;
}