//
//  Dialogs.cpp
//  Gems
//
//  Created by Sunil Patel on 11/11/2015.
//
//

#include "Popup.hpp"
#include "Dialogs.hpp"
#include "Strings.hpp"
#include "Constants.h"
#include "GameScene.hpp"
#include "GameController.hpp"
#include "SoundManager.hpp"

#include "ui/CocosGUI.h"

bool OptionsDialog::init() {
	auto size = Size(300, 300);
	if ( !Dialog::init(true, true, size.width, size.height) ) {
		return false;
	}
	
	auto popup = Popup::create(size.width, size.height);
	popup->setPosition(size/-2);
	this->addChild(popup);
	setContentSize(size);

	auto label = Label::createWithTTF( "Options", Fonts::TITLE_FONT, Fonts::TITLE_SIZE);
	label->setColor(Color3B::BLACK);
	label->setPosition(Vec2(0, 80));
	this->addChild(label, 1);
	
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


bool PreLevelDialog::init(RoundDef *round) {
	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto size = Size(visibleSize.width - 50, 300);
	
	if ( !Dialog::init(true, false, size.width, size.height) ) {
		return false;
	}
	
	auto popup = Popup::create(size.width, size.height);
	popup->setPosition(size/-2);
	this->addChild(popup);
	setContentSize(size);
	
	auto label = Label::createWithTTF( round->name, Fonts::TITLE_FONT, Fonts::TITLE_SIZE);
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
	button->setPosition(Vec2(0, -size.height/2));
	button->setTitleText _("ui.PLAY");
	button->addTouchEventListener([round](Ref* pSender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
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
			SoundManager::get()->playEffect(kSoundEffect_UIBack);
			gc->popDialog();
		}
	}
	return true;
}
void Dialog::onTouchMoved(cocos2d::Touch *, cocos2d::Event *) {
}
void Dialog::onTouchEnded(cocos2d::Touch *, cocos2d::Event *) {
}
