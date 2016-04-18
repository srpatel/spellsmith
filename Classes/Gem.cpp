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
		case AIR: element = "gems/air.png"; break;
		case EARTH: element = "gems/earth.png"; break;
		case FIRE: element = "gems/fire.png"; break;
		case WATER: element = "gems/water.png"; break;
		case NONE: /*hopefully won't happen!*/ break;
	}
	this->type = type;
	
	sprite = LoadSprite(element);
	//sprite->setScale(Gem::scale);
}

void Gem::init() {
    init(static_cast<GemType>(1 + rand() % 4));
}

Size Gem::getSize() {
	// TODO : Make this dynamic
	//auto frame = SPRITE("gems/air_01.png");
	//return frame->getRect().size * scale;
	return Size(80, 100) * scale;
}