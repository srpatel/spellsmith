//
//  Dialogs.cpp
//  Gems
//
//  Created by Sunil Patel on 11/11/2015.
//
//

#include "Dialogs.hpp"
#include "GameScene.hpp"

bool LevelEndDialog::init() {
	if ( !Dialog::init() ) {
		return false;
	}
	
	auto background = cocos2d::DrawNode::create();
	float xmax = 100;
	float ymax = 100;
	background->drawSolidRect(Vec2(-xmax, -ymax), Vec2(xmax, ymax), Color4F::WHITE);
	addChild(background);
	
	return true;
}

bool SpellInfoDialog::init() {
	if ( !Dialog::init() ) {
		return false;
	}
	
	auto background = cocos2d::DrawNode::create();
	float xmax = 100;
	float ymax = 100;
	background->drawSolidRect(Vec2(-xmax, -ymax), Vec2(xmax, ymax), Color4F::WHITE);
	addChild(background);
	
	return true;
}

bool Dialog::init() {
	if ( !Layer::init() ) {
		return false;
	}
	
	auto touchListener = cocos2d::EventListenerTouchOneByOne::create();
	
	touchListener->onTouchBegan = CC_CALLBACK_2(SpellInfoDialog::onTouchBegan, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(SpellInfoDialog::onTouchEnded, this);
	touchListener->onTouchMoved = CC_CALLBACK_2(SpellInfoDialog::onTouchMoved, this);
	//    touchListener->onTouchCancelled = CC_CALLBACK_2(Grid::onTouchCancelled, this);
	
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
	
	return true;
}

bool Dialog::onTouchBegan(cocos2d::Touch *, cocos2d::Event *event) {
	// Close the dialog!
	event->stopPropagation();
	Game::get()->removeChild(this);
	return false;
}
void Dialog::onTouchMoved(cocos2d::Touch *, cocos2d::Event *) {
}
void Dialog::onTouchEnded(cocos2d::Touch *, cocos2d::Event *) {
}