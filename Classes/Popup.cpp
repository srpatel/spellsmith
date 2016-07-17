//
//  Popup.cpp
//  Gems
//
//  Created by Sunil Patel on 17/07/2016.
//
//

#include "Popup.hpp"
#include "Constants.h"


bool Popup::init(float width, float height) {
	if ( !Layer::init() ) {
		return false;
	}
	
	setContentSize(Size(width, height));
	
	auto top_left = LoadSprite("popup/top_left.png");
	top_left->setAnchorPoint(Vec2(0, 1));
	top_left->setPosition(Vec2(0, getContentSize().height));
	addChild(top_left);
	
	auto top_right = LoadSprite("popup/top_right.png");
	top_right->setAnchorPoint(Vec2(1, 1));
	top_right->setPosition(getContentSize());
	addChild(top_right);
	
	auto bottom_right = LoadSprite("popup/bottom_right.png");
	bottom_right->setAnchorPoint(Vec2(1, 0));
	bottom_right->setPosition(Vec2(getContentSize().width, 0));
	addChild(bottom_right);
	
	auto bottom_left = LoadSprite("popup/bottom_left.png");
	bottom_left->setAnchorPoint(Vec2::ZERO);
	bottom_left->setPosition(Vec2::ZERO);
	addChild(bottom_left);
	
	// Colour blocks:
	auto offset = bottom_left->getContentSize().height;
	auto horizontal_block = LayerColor::create();
	horizontal_block->initWithColor(Color4B(Colours::UI_BACKGROUND));
	horizontal_block->setPosition(Vec2(0, offset));
	horizontal_block->setContentSize(Size(width, height - offset*2));
	addChild(horizontal_block);
	
	auto vertical_block = LayerColor::create();
	vertical_block->initWithColor(Color4B(Colours::UI_BACKGROUND));
	vertical_block->setPosition(Vec2(offset, 0));
	vertical_block->setContentSize(Size(width - offset*2, height));
	addChild(vertical_block);
	
	// Edges
	auto left_edge = LoadSprite("popup/left_edge.png");
	left_edge->setAnchorPoint(Vec2(0, 0));
	left_edge->setPosition(Vec2(0, offset));
	left_edge->setScaleY(Director::getInstance()->getContentScaleFactor()*(height - 2*offset));
	addChild(left_edge);
	
	auto right_edge = LoadSprite("popup/right_edge.png");
	right_edge->setAnchorPoint(Vec2(1, 0));
	right_edge->setPosition(Vec2(width, offset));
	right_edge->setScaleY(Director::getInstance()->getContentScaleFactor()*(height - 2*offset));
	addChild(right_edge);
	
	auto top_edge = LoadSprite("popup/top_edge.png");
	top_edge->setAnchorPoint(Vec2(0, 1));
	top_edge->setPosition(Vec2(offset, height));
	top_edge->setScaleX(Director::getInstance()->getContentScaleFactor()*(width - 2*offset));
	addChild(top_edge);
	
	auto bottom_edge = LoadSprite("popup/bottom_edge.png");
	bottom_edge->setAnchorPoint(Vec2(0, 0));
	bottom_edge->setPosition(Vec2(offset, 0));
	bottom_edge->setScaleX(Director::getInstance()->getContentScaleFactor()*(width - 2*offset));
	addChild(bottom_edge);
	
	return true;
}