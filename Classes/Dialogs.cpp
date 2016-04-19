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
#include "GameController.hpp"

#include "ui/CocosGUI.h"

bool LevelWonDialog::init() {
	float xmax = 150;
	float ymax = 150;
	if ( !Dialog::init(false, xmax*2, ymax*2) ) {
		return false;
	}
	
	auto background = cocos2d::DrawNode::create();
	background->drawSolidRect(Vec2(-xmax, -ymax), Vec2(xmax, ymax), Color4F::WHITE);
	addChild(background);
	
	// stars!
		// if you won, they animate in 1/2/3 of them
		// if you'd already won it, the previous is shown somewhere?
	// "congratulations!" or "Unlucky!"
	// some stats?
	// if lost: "retry": button
	// "map" button
	
	auto label = Label::createWithTTF( _("ui.LEVEL_WON"), Fonts::TITLE_FONT, Fonts::TITLE_SIZE);
	label->setColor(Color3B::BLACK);
	label->setPosition(Vec2(0, 80));
	this->addChild(label, 1);
	
	{
		auto button = ui::Button::create("ui/button.png", "ui/buttondown.png", "ui/buttondisabled.png", TEXTURE_TYPE);
		button->setTitleFontName(Fonts::TEXT_FONT);
		button->setTitleText _("ui.RETURN_TO_MAP");
		
		button->setPosition(Vec2(0, -80));
		button->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType type) {
			if (type == ui::Widget::TouchEventType::ENDED) {
				auto gc = GameController::get();
				gc->popDialog();
				gc->setState(kStateMainMenu);
			}
		});
		this->addChild(button);
	}
	
	return true;
}

bool LevelLostDialog::init() {
	float xmax = 150;
	float ymax = 150;
	
	if ( !Dialog::init(false, xmax*2, ymax*2) ) {
		return false;
	}
	
	auto background = cocos2d::DrawNode::create();
	
	background->drawSolidRect(Vec2(-xmax, -ymax), Vec2(xmax, ymax), Color4F::WHITE);
	addChild(background);
	
	// stars!
	// if you won, they animate in 1/2/3 of them
	// if you'd already won it, the previous is shown somewhere?
	// "congratulations!" or "Unlucky!"
	// some stats?
	// if lost: "retry": button
	// "map" button
	
	auto label = Label::createWithTTF( _("ui.LEVEL_LOST"), Fonts::TITLE_FONT, Fonts::TITLE_SIZE);
	label->setColor(Color3B::BLACK);
	label->setPosition(Vec2(0, 80));
	this->addChild(label, 1);
	
	{
		auto button = ui::Button::create("ui/button.png", "ui/buttondown.png", "ui/buttondisabled.png", TEXTURE_TYPE);
		button->setTitleFontName(Fonts::TEXT_FONT);
		button->setTitleText _("ui.RETRY_LEVEL");
		
		button->setPosition(Vec2(0, -40));
		button->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType type) {
			if (type == ui::Widget::TouchEventType::ENDED) {
				auto gc = GameController::get();
				gc->popDialog();
				
				// Reset game state
				Game::get()->startGame(nullptr);
			}
		});
		this->addChild(button);
	}
	
	{
		auto button = ui::Button::create("ui/button.png", "ui/buttondown.png", "ui/buttondisabled.png", TEXTURE_TYPE);
		button->setTitleFontName(Fonts::TEXT_FONT);
		button->setTitleText _("ui.RETURN_TO_MAP");
		
		button->setPosition(Vec2(0, -80));
		button->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType type) {
			if (type == ui::Widget::TouchEventType::ENDED) {
				auto gc = GameController::get();
				gc->popDialog();
				gc->setState(kStateMainMenu);
			}
		});
		this->addChild(button);
	}
	
	return true;
}

bool SpellInfoDialog::init(Spell *spell) {
	float xmax = 100;
	float ymax = 100;
	if ( !Dialog::init(true, xmax*2, ymax*2) ) {
		return false;
	}
	
	auto background = DrawNode::create();
	background->drawSolidRect(Vec2(-xmax, -ymax), Vec2(xmax, ymax), Color4F::WHITE);
	addChild(background);
	
	auto label = Label::createWithTTF( spell->getName(), Fonts::TITLE_FONT, Fonts::TITLE_SIZE);
	label->setColor(Color3B::BLACK);
	label->setPosition(Vec2(0, 80));
	this->addChild(label, 1);
	
	auto d = spell->getDescription();
	ui::Text* text = ui::Text::create( spell->getDescription(), Fonts::TEXT_FONT, Fonts::TEXT_SIZE);
	text->setColor(Color3B::BLACK);
	text->setPosition(Vec2(0, -80));
	this->addChild(text, 1);
	
	// Draw gems in the middle of the dialog
	
	// Put description at the bottom.
	
	return true;
}

bool Dialog::init(bool closeable, float centralWidth, float centralHeight) {
	if ( !Layer::init() ) {
		return false;
	}
	this->closeable = closeable;
	this->centralWidth = centralWidth;
	this->centralHeight = centralHeight;
	
	auto size = Director::getInstance()->getVisibleSize();
	
	auto background = cocos2d::DrawNode::create();
	background->drawSolidRect(Vec2(size / -2), Vec2(size / 2), Color4F(Colours::SEMIBLACK));
	addChild(background);
	
	if (closeable) {
		auto closebutton = LoadSprite("ui/cross.png");
		closebutton->setAnchorPoint(Vec2(1, 1));
		closebutton->setPosition(Vec2(size.width/2 - 10, size.height/2 - 10));
		this->addChild(closebutton, 1);
	}
	
	auto touchListener = cocos2d::EventListenerTouchOneByOne::create();
	touchListener->setSwallowTouches(true);
	touchListener->onTouchBegan = CC_CALLBACK_2(Dialog::onTouchBegan, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(Dialog::onTouchEnded, this);
	touchListener->onTouchMoved = CC_CALLBACK_2(Dialog::onTouchMoved, this);
	//    touchListener->onTouchCancelled = CC_CALLBACK_2(Grid::onTouchCancelled, this);
	
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
	
	return true;
}

bool Dialog::onTouchBegan(Touch *touch, Event *event) {
	event->stopPropagation();
	if (closeable) {
		// don't close if clicking the dialog!
		auto size = Director::getInstance()->getVisibleSize();
		auto bounds = Rect(
			(size.width - centralWidth)/2,
			(size.height - centralHeight)/2,
			centralWidth,
			centralHeight
		);
		if (! bounds.containsPoint(touch->getLocation())) {
			auto gc = GameController::get();
			gc->popDialog();
		}
	}
	return true;
}
void Dialog::onTouchMoved(cocos2d::Touch *, cocos2d::Event *) {
}
void Dialog::onTouchEnded(cocos2d::Touch *, cocos2d::Event *) {
}