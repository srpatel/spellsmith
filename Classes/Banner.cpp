//
//  Banner.cpp
//  Gems
//
//  Created by Sunil Patel on 09/09/2017.
//
//

#include "Banner.hpp"
#include "Constants.h"

#include "ui/CocosGUI.h"

bool Banner::init(std::string text, float width) {
	if ( !Layer::init() ) {
		return false;
	}
	
	setCascadeOpacityEnabled(true);
	
	std::string uctext{text};
	std::transform(uctext.begin(), uctext.end(), uctext.begin(), ::toupper);
	
	auto label = Label::createWithTTF( uctext, Fonts::INTERESTING_FONT, Fonts::TITLE_SIZE );
	label->setColor(Color3B::BLACK);
	label->setOpacity(125);
	//label->enableOutline(Color4B::BLACK, 1);
	label->setPosition({0, 5});
	this->addChild(label, 1);
	
	if (width <= 0) {
		width = label->getContentSize().width + 35;
	} else {
		TTFConfig config{label->getTTFConfig()};
		while (width < label->getContentSize().width + 35) {
			config.fontSize -= 2;
			label->setTTFConfig(config);
		}
	}
	
	setContentSize(Size(width, 60));
	
	auto sprite = ui::Scale9Sprite::createWithSpriteFrame(
		LoadSpriteFrame("popup/banner9.png"),
		Rect(39, 19, 12, 9)
	);
	sprite->setContentSize(getContentSize());
	sprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	addChild(sprite);
	
	return true;
}
