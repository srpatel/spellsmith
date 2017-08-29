//
//  Dialogs.cpp
//  Gems
//
//  Created by Sunil Patel on 11/11/2015.
//
//

#include "Popup.hpp"
#include "Dialogs.hpp"
#include "Tutorial.hpp"
#include "Strings.hpp"
#include "Constants.h"
#include "GameScene.hpp"
#include "GameController.hpp"
#include "SoundManager.hpp"

#include "ui/CocosGUI.h"

bool OptionsDialog::init() {
	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto size = Size(visibleSize.width - 50, 300);
	if ( !Dialog::init(true, true, size.width, size.height) ) {
		return false;
	}
	
	auto popup = Popup::create(size.width, size.height);
	popup->setPosition(size/-2);
	this->addChild(popup);
	setContentSize(size);

	auto label = Label::createWithTTF( _("ui.OPTIONS"), Fonts::TITLE_FONT, Fonts::TITLE_SIZE);
	label->setColor(Color3B::BLACK);
	label->setPosition(Vec2(0, size.height/2 - Fonts::TITLE_SIZE));
	this->addChild(label, 1);
	
	// Mute button
	// ???
	
	// Return to menu
	auto button = ui::Button::create("ui/button_up.png", "ui/button_down.png", "ui/button_down.png", TEXTURE_TYPE);
	button->setTitleFontName(Fonts::TEXT_FONT);
	button->setTitleFontSize(Fonts::TEXT_SIZE);
	button->setPosition(Vec2(0, 30-size.height/2));
	button->setTitleText _("ui.RETURN_TO_MAP");
	button->addTouchEventListener([button](Ref* pSender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			button->setTouchEnabled(false);
			PLAY_SOUND( kSoundEffect_UISelect );
			GameController::get()->popDialog();
			GameController::get()->setState(kStateMap);
		}
	});
	this->addChild(button);
	
	return true;
}

bool SpellInfoDialog::init(Spell *spell) {
	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto size = Size(visibleSize.width - 50, 300);
	
	if ( !Dialog::init(true, false, size.width, size.height) ) {
		return false;
	}
	
	auto popup = Popup::create(size.width, size.height);
	popup->setPosition(size/-2);
	this->addChild(popup);
	setContentSize(size);
	
	auto label = Label::createWithTTF( spell->getName(), Fonts::TITLE_FONT, Fonts::TITLE_SIZE);
	label->setColor(Color3B::BLACK);
	label->setPosition(Vec2(0, size.height/2 - Fonts::TITLE_SIZE));
	this->addChild(label, 1);
	
	auto d = spell->getDescription();
	ui::Text* text = ui::Text::create( spell->getDescription(), Fonts::TEXT_FONT, Fonts::TEXT_SIZE);
	text->setColor(Color3B::BLACK);
	text->setPosition(Vec2(0, 15));
	text->ignoreContentAdaptWithSize(false);
	text->setContentSize(Size(size.width - 20, size.height));
	text->setTextHorizontalAlignment(TextHAlignment::CENTER);
	text->setTextVerticalAlignment(TextVAlignment::BOTTOM);
	this->addChild(text, 1);
	
	// Draw gems in the middle of the dialog
	auto gems = spell->makeNode(false);
	addChild(gems);
	
	return true;
}

bool Dialog::init(bool closeable, bool captureTouch, float centralWidth, float centralHeight) {
	if ( !Layer::init() ) {
		return false;
	}
	this->captureTouch = captureTouch;
	this->closeable = closeable;
	this->centralWidth = centralWidth;
	this->centralHeight = centralHeight;
	
	auto size = Director::getInstance()->getVisibleSize();
	
	auto background = LayerColor::create();
	background->initWithColor({0, 0, 0, 180});
	background->setContentSize(size);
	background->setPosition(size/-2);
	addChild(background);
	
	if (closeable) {
		auto ttc = Label::createWithTTF(_("ui.TAP_TO_CLOSE"), Fonts::TEXT_FONT, Fonts::SMALL_SIZE);
		ttc->setDimensions(size.width - 20, 0);
		ttc->setColor(Color3B::WHITE);
		ttc->setAlignment(TextHAlignment::CENTER);
		// position the label on the center of the screen
		ttc->setPosition(Vec2(0, -size.height / 2 + ttc->getContentSize().height));
		addChild(ttc);
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


bool PreLevelDialog::init(RoundDef *round) {
	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto size = Size(visibleSize.width - 50, 300);
	
	if ( !Dialog::init(true, true, size.width, size.height) ) {
		return false;
	}
	
	auto popup = Popup::create(size.width, size.height);
	popup->setPosition(size/-2);
	this->addChild(popup);
	setContentSize(size);
	
	auto roundName = _(std::string("level.")+round->name);
	auto label = Label::createWithTTF( roundName, Fonts::TITLE_FONT, Fonts::TITLE_SIZE);
	label->setColor(Color3B::BLACK);
	label->setPosition(Vec2(0, size.height/2 - Fonts::TITLE_SIZE));
	this->addChild(label, 1);
	
	// Add spell icons
	float currentX = -50 * ((float) round->rewards.size() - 1) / 2.0;
	for (std::string spellname : round->rewards) {
		auto spell = SpellManager::get()->getByName(spellname);
		if (spell) {
			auto node = spell->makeNode(true);
			node->setPositionX(currentX);
			currentX += 50;
			// TODO : on click, get info
			// TODO : organise layout of multiple
			addChild(node);
		}
	}
	
	// Add monsters
	// ...
	
	// Add play button
	auto button = ui::Button::create("ui/button_up.png", "ui/button_down.png", "ui/button_down.png", TEXTURE_TYPE);
	button->setTitleFontName(Fonts::TEXT_FONT);
	button->setTitleFontSize(Fonts::TEXT_SIZE);
	button->setPosition(Vec2(0, 30-size.height/2));
	button->setTitleText _("ui.PLAY");
	button->addTouchEventListener([round, button](Ref* pSender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			button->setTouchEnabled(false);
			PLAY_SOUND( kSoundEffect_UISelect );
			GameController::get()->startRound(round);
		}
	});
	this->addChild(button);
	
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
		if (! captureTouch || ! bounds.containsPoint(touch->getLocation())) {
			auto gc = GameController::get();
			PLAY_SOUND(kSoundEffect_UIBack);
			gc->popDialog();
			Tutorial::activate(9);
		}
	}
	return true;
}
void Dialog::onTouchMoved(cocos2d::Touch *, cocos2d::Event *) {
}
void Dialog::onTouchEnded(cocos2d::Touch *, cocos2d::Event *) {
}
