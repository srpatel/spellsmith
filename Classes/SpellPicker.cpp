//
//  SpellPicker.cpp
//  Gems
//
//  Created by Sunil Patel on 10/07/2016.
//
//

#include "SpellBlob.hpp"
#include "SpellPicker.hpp"
#include "GameController.hpp"
#include "Constants.h"
#include "Popup.hpp"
#include "GameScene.hpp"
#include "SoundManager.hpp"

#include "ui/CocosGUI.h"

bool SpellPicker::init(Spell *s1, Spell *s2) {
	if ( !Layer::init() ) {
		return false;
	}
	
	auto size = Size(200, 150);
	auto popup = Popup::create(size.width, size.height);
	popup->setPosition(size/-2);
	this->addChild(popup);
	setContentSize(size);
	
	// "Pick a spell" title
	auto label = Label::createWithTTF( "Choose a Spell", Fonts::TITLE_FONT, Fonts::TEXT_SIZE);
	label->setColor(Color3B::WHITE);
	label->setPosition(Vec2(0, 50));
	addChild(label, 1);
	
	auto label2 = Label::createWithTTF( "Tap for info. Drag to a slot to learn.", Fonts::TITLE_FONT, Fonts::SMALL_SIZE);
	label2->setColor(Color3B::WHITE);
	label2->setPosition(Vec2(0, -50));
	addChild(label2, 1);
	
	auto sb1 = SpellBlob::create(s1, true);
	sb1->setPosition(5-getContentSize().width/4, -5);
	addChild(sb1, 2);
	
	auto sb2 = SpellBlob::create(s2, true);
	sb2->setPosition(-5+getContentSize().width/4, -5);
	addChild(sb2, 2);
	
	return true;
}

bool PostLevelDialog::init(RoundDef *r) {
	if ( !Layer::init() ) {
		return false;
	}
	
	auto size = Size(200, 150);
	auto popup = Popup::create(size.width, size.height);
	popup->setPosition(size/-2);
	this->addChild(popup);
	setContentSize(size);
	
	// "Pick a spell" title
	auto label = Label::createWithTTF( "You have learnt:", Fonts::TITLE_FONT, Fonts::TEXT_SIZE);
	label->setColor(Color3B::WHITE);
	label->setPosition(Vec2(0, 50));
	addChild(label, 1);
	
	// Add spell blobs
	int numRewards = r->rewards.size();
	float dx = (getContentSize().width/2) - 10;
	float startX = - (dx * (numRewards - 1)) / 2.0f;
	for (std::string spellname : r->rewards) {
		Spell *s = SpellManager::get()->getByName(spellname);
		auto sb = SpellBlob::create(s, false);
		sb->setPosition(startX, -5);
		startX += dx;
		addChild(sb);
	}
	
	// Add button -> spellbook
	auto button = ui::Button::create("ui/button_up.png", "ui/button_down.png", "ui/button_down.png", TEXTURE_TYPE);
	button->setTitleFontName(Fonts::TEXT_FONT);
	button->setTitleText("Spellbook");
	button->setPosition(Vec2(0, -50));
	button->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			// For now, from scratch always!
			removeFromParent();
			GameController::get()->setState(kStateSpellbook);
		}
	});
	addChild(button);
	
	return true;
}
