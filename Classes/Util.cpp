//
//  Util.cpp
//  Gems
//
//  Created by Sunil Patel on 30/08/2017.
//
//

#include "Util.hpp"
#include "Constants.h"
#include "Popup.hpp"

Layer *makeLargeTemporaryTextPopup(std::string text) {
	auto yBuffer = 50;
	auto size = Size(200, 0);
	
	// Add wrapping text in the middle...
	auto label = Label::createWithTTF(text, Fonts::TEXT_FONT, Fonts::TITLE_SIZE);
	label->setDimensions(size.width - 20, 0);
	label->setAlignment(TextHAlignment::CENTER);
	size.height = label->getContentSize().height + yBuffer;
	// position the label on the center of the screen
	label->setPosition(size/2 + Size(0, 0));
	
	auto popup = Popup::create(size.width, size.height);
	popup->setCascadeOpacityEnabled(true);
	popup->addChild(label);
	
	Vec2 position;
	
	popup->setOpacity(0);
	auto fadeIn = Sequence::create(
		FadeIn::create(0.5),
		DelayTime::create(1),
		FadeOut::create(0.5),
		RemoveSelf::create(),
		nullptr);
	popup->runAction(fadeIn);
	
	return popup;
}
