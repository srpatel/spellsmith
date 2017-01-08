//
//  TextWisp.cpp
//  Gems
//
//  Created by Sunil Patel on 08/01/2017.
//
//

#include "TextWisp.hpp"
#include "Constants.h"

bool TextWisp::init(std::string text, cocos2d::Color3B colour) {
	if ( !Layer::init() ) {
		return false;
	}
	// add and colour the text
	auto label = Label::createWithTTF( text, Fonts::TITLE_FONT, Fonts::TEXT_SIZE);
	label->setColor(colour);
	label->setPosition(Vec2(0, 0));
	addChild(label, 1);
	
	// add an animation which animates us up!
	auto waitTime = 0.1f;
	auto wispTime = 0.5f;
	label->runAction(Sequence::create(
		DelayTime::create(waitTime),
		Spawn::create(
			EaseIn::create(MoveBy::create(wispTime, Vec2(0, 15)), 0.5f),
			FadeOut::create(wispTime),
			nullptr),
		RemoveSelf::create(),
		nullptr));
	runAction(Sequence::create(
		DelayTime::create(waitTime + wispTime),
		RemoveSelf::create(),
		nullptr));
	return true;
}
