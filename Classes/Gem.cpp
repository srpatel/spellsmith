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
		case CRYSTAL: element = "gems/gold.png"; break;
		case NONE: /*hopefully won't happen!*/ break;
	}
	this->type = type;
	
	
	
	/*if (type == CRYSTAL) {
		sprite = Sprite::create();
		sprite->setContentSize(getSize());
		// Animate it!
		auto size = 8;
		Vector<SpriteFrame*> animFrames(size);
		char str[100] = {0};
		for(int i = 1; i <= size; i++)
		{
			sprintf(str, "gems/crystal_%02d.png", i);
			auto frame = LoadSpriteFrame( std::string(str), Rect(0, 0, 80 / CC_CONTENT_SCALE_FACTOR(), 100 / CC_CONTENT_SCALE_FACTOR()) );
			animFrames.pushBack(frame);
		}
		
		auto animation = Animation::createWithSpriteFrames(animFrames, 0.1f);
		auto animate = RepeatForever::create(Sequence::create(Animate::create(animation), DelayTime::create(01.2f), nullptr));
		sprite->runAction(animate);
	} else */
	{
		sprite = LoadSprite(element);
	}
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