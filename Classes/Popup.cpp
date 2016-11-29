//
//  Popup.cpp
//  Gems
//
//  Created by Sunil Patel on 17/07/2016.
//
//

#include "Popup.hpp"
#include "Constants.h"

#include "ui/CocosGUI.h"

bool Popup::init(float width, float height) {
	if ( !Layer::init() ) {
		return false;
	}
	
	setContentSize(Size(width, height));
	
	//auto sf = Director::getInstance()->getContentScaleFactor();
	auto sprite = ui::Scale9Sprite::createWithSpriteFrame(
		LoadSpriteFrame("popup/popup9.png"),
		Rect(12, 12, 7, 9)
	);
	sprite->setContentSize(getContentSize());
	sprite->setAnchorPoint(Vec2::ZERO);
	addChild(sprite);
	
	return true;
}
