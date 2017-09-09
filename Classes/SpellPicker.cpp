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
#include "Strings.hpp"
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
	auto label = Label::createWithTTF( _("level.CHOOSE_SPELL"), Fonts::TITLE_FONT, Fonts::TEXT_SIZE);
	label->setColor(Color3B::WHITE);
	label->setPosition(Vec2(0, 50));
	addChild(label, 1);
	
	auto label2 = Label::createWithTTF( _("level.CHOOSE_DESC"), Fonts::TITLE_FONT, Fonts::SMALL_SIZE);
	label2->setColor(Color3B::WHITE);
	label2->setPosition(Vec2(0, -50));
	addChild(label2, 1);
	
	auto onPickSpell = [this](int invPosition, Spell *selected){
		Game::get()->getWizard()->inventory[invPosition] = selected;
		Game::get()->updateInventory();
		
		Game::get()->spellPicked();
		removeFromParent();
	};
	
	auto isBeingUsed = [this](Spell *checkme) -> bool{
		auto inv = Game::get()->getWizard()->inventory;
		for (int i = 0; i < 6; i++) {
			if (inv[i] == checkme)
				return true;
		}
		return false;
	};
	
	auto sb1 = SpellBlob::create(s1, false, true, onPickSpell, isBeingUsed);
	sb1->setPosition(5-getContentSize().width/4, 5);
	addChild(sb1, 2);
	
	auto sb2 = SpellBlob::create(s2, false, true, onPickSpell, isBeingUsed);
	sb2->setPosition(-5+getContentSize().width/4, 5);
	addChild(sb2, 2);
	
	return true;
}

bool PostLevelDialog::init(RoundDef *r) {
	if ( !Layer::init() ) {
		return false;
	}
	
	// "Pick a spell" title
	auto label = Label::createWithTTF( _("level.YOU_HAVE_LEARNT"), Fonts::TITLE_FONT, Fonts::TEXT_SIZE);
	label->setColor(Color3B::WHITE);
	addChild(label, 1);
	
	// Add button -> spellbook
	auto button = ui::Button::create("ui/button_up.png", "ui/button_down.png", "ui/button_down.png", TEXTURE_TYPE);
	button->setTitleFontName(Fonts::TEXT_FONT);
	button->setTitleFontSize(Fonts::TEXT_SIZE);
	button->setTitleText _("ui.SPELLBOOK");
	button->addTouchEventListener([this](Ref* pSender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			// For now, from scratch always!
			removeFromParent();
			GameController::get()->setState(kStateSpellbook);
		}
	});
	addChild(button, 1);

	auto size = Size(
		200,
		10 +
		label->getContentSize().height +
		5 +
		80 +
		button->getContentSize().height +
		15
	);
	label->setPosition({0, size.height/2 - (10 + label->getContentSize().height/2)});
	button->setPosition({0, -size.height/2 + 15 + button->getContentSize().height/2});
	
	// Add spell blobs
	int numRewards = r->rewards.size();
	float dx = (getContentSize().width/2) - 10;
	float startX = - (dx * (numRewards - 1)) / 2.0f;
	for (std::string spellname : r->rewards) {
		Spell *s = SpellManager::get()->getByName(spellname);
		auto sb = SpellBlob::create(s, false, false, nullptr, nullptr);
		sb->setPosition(startX, button->getPosition().y + button->getContentSize().height/2 + 50);
		startX += dx;
		addChild(sb, 1);
	}
	
	auto popup = Popup::create(size.width, size.height);
	popup->setPosition(size/-2);
	this->addChild(popup);
	
	setContentSize(size);
	
	return true;
}
