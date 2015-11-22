//
//  Dialogs.cpp
//  Gems
//
//  Created by Sunil Patel on 11/11/2015.
//
//

#include "Dialogs.hpp"
#include "Strings.hpp"
#include "Constants.h"
#include "GameScene.hpp"

#include "ui/CocosGUI.h"

bool LevelEndDialog::init() {
	if ( !Dialog::init() ) {
		return false;
	}
	
	auto background = cocos2d::DrawNode::create();
	float xmax = 100;
	float ymax = 100;
	background->drawSolidRect(Vec2(-xmax, -ymax), Vec2(xmax, ymax), Color4F::WHITE);
	addChild(background);
	
	// stars!
		// if you won, they animate in 1/2/3 of them
		// if you'd already won it, the previous is shown somewhere?
	// "congratulations!" or "Unlucky!"
	// some stats?
	// if lost: "retry": button
	// "map" button
	
	auto label = Label::createWithTTF( _("win/lose"), Fonts::MAIN_FONT, 32);
	label->setColor(Color3B::BLACK);
	label->setPosition(Vec2(0, 80));
	this->addChild(label, 1);
	
	// Play button
	auto button = ui::Button::create("ui/button.png", "ui/button.png", "ui/button.png", ui::Widget::TextureResType::PLIST);
	button->setTitleFontName(Fonts::MAIN_FONT);
	button->setTitleText _("Back");
	
	button->setPosition(Vec2(0, -80));
	button->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::BEGAN) {
			getParent()->removeChild(this);
		}
	});
	this->addChild(button);
	
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
	
	//_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
	
	return true;
}

bool Dialog::onTouchBegan(cocos2d::Touch *, cocos2d::Event *event) {
	// Close the dialog!
	event->stopPropagation();
	getParent()->removeChild(this);
	return false;
}
void Dialog::onTouchMoved(cocos2d::Touch *, cocos2d::Event *) {
}
void Dialog::onTouchEnded(cocos2d::Touch *, cocos2d::Event *) {
}