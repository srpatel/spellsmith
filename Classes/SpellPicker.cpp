//
//  SpellPicker.cpp
//  Gems
//
//  Created by Sunil Patel on 10/07/2016.
//
//

#include "SpellPicker.hpp"
#include "GameController.hpp"
#include "Constants.h"
#include "Popup.hpp"
#include "GameScene.hpp"

#include "ui/CocosGUI.h"

class SpellBlob : public Layer {
public:
	bool init(Spell *, bool draggable);
	CREATE_FUNC_2(SpellBlob, Spell *, bool);
private:
	bool draggable;
	float distanceMoved;
};

bool SpellBlob::init(Spell *spell, bool draggable) {
	if ( !Layer::init() ) {
		return false;
	}
	
	this->draggable = draggable;

	auto bg = LoadSprite("ui/spellbox.png");
	bg->setAnchorPoint(Vec2(0.5, 0.5));
	bg->setPosition(Vec2(0, 0));
	this->addChild(bg);
	setContentSize(bg->getContentSize());
	
	auto label = Label::createWithTTF( spell->getName(), Fonts::TEXT_FONT, Fonts::SMALL_SIZE);
	label->setColor(Color3B::WHITE);
	label->setPosition(Vec2(0, -getContentSize().height/2-8));
	this->addChild(label, 1);
	
	// TODO - need to make sure we don't add this in two places at once?
	auto mininode = spell->mininode;
	mininode->setPosition(Vec2::ZERO);
	addChild(mininode);
	
	auto onClick = EventListenerTouchOneByOne::create();
	onClick->setSwallowTouches(true);
	onClick->onTouchBegan = [this](Touch* touch, Event* event) -> bool {
		distanceMoved = 0;
		
		auto bounds = event->getCurrentTarget()->getBoundingBox();
		bounds.origin -= bounds.size/2;
		bounds.origin += this->getParent()->getPosition();
		
		if (bounds.containsPoint(touch->getLocation())){
			// If it's not draggable, show the spell info dialog straight away.
			// Otherwise, wait for the release.
			if (! draggable)
				GameController::get()->showSpellInfoDialog(spell);
			return true;
		}
		
		return false; // if you are consuming it
	};
	if (draggable) {
		onClick->onTouchMoved = [this, mininode](Touch* touch, Event* event) -> bool {
			auto offset = this->getParent()->getPosition() + getPosition();
			mininode->setPosition(touch->getLocation() - offset);
			distanceMoved += touch->getDelta().length();
			// move mini-node to new location
			return false; // if you are consuming it
		};
		onClick->onTouchEnded = [this, mininode, spell](Touch* touch, Event* event) -> bool {
			auto bounds = event->getCurrentTarget()->getBoundingBox();
			bounds.origin -= bounds.size/2;
			bounds.origin += this->getParent()->getPosition();
			
			bool doSnap = true;
			if (bounds.containsPoint(touch->getLocation())){
				// If haven't moved far, then do this:
				if (distanceMoved < 2.0f) {
					GameController::get()->showSpellInfoDialog(spell);
				}
			} else {
				// if it's over a inventory spot, put the gem there
				layout_t layout = Game::get()->getLayout();
				const auto size = getContentSize();
				const float starty = layout.column_height - 110 * layout.ui_scale;
				for (int i = 0; i < 3; i++) {
					auto bounds = Rect(
						Vec2(18 * layout.ui_scale - size.width/2, starty - i * 55 * layout.ui_scale - size.height/2),
						size
					);
					if (bounds.containsPoint(touch->getLocation())) {
						// Set inventory to this spell
						Game::get()->getWizard()->inventory[i] = spell;
						mininode->removeFromParent();
						Game::get()->updateInventory();
						doSnap = false;
					}
				}
				for (int i = 0; i < 3; i++) {
					auto bounds = Rect(
									   Vec2(Game::get()->getBoundingBox().size.width - 18 * layout.ui_scale - size.width/2, starty - i * 55 * layout.ui_scale - size.height/2),
									   size
									   );
					if (bounds.containsPoint(touch->getLocation())) {
						// Set inventory to this spell
						Game::get()->getWizard()->inventory[3 + i] = spell;
						mininode->removeFromParent();
						Game::get()->updateInventory();
						doSnap = false;
					}
				}
			}
			
			if (doSnap) {
				// Snap back to original position.
				auto snap = MoveTo::create(0.1f, Vec2::ZERO);
				mininode->stopAllActions();
				mininode->runAction(snap);
			} else {
				Game::get()->spellPicked();
				getParent()->removeFromParent();
			}
			
			return true; // if you are consuming it
		};
	}
	_eventDispatcher->addEventListenerWithSceneGraphPriority(onClick, this);
	
	return true;
}

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
	addChild(sb1);
	
	auto sb2 = SpellBlob::create(s2, true);
	sb2->setPosition(-5+getContentSize().width/4, -5);
	addChild(sb2);
	
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
	
	auto label2 = Label::createWithTTF( "Tap for info.", Fonts::TITLE_FONT, Fonts::SMALL_SIZE);
	label2->setColor(Color3B::WHITE);
	label2->setPosition(Vec2(0, -50));
	addChild(label2, 1);
	
	// Add spell blobs
	int numRewards = r->rewards.size();
	float dx = (getContentSize().width/2) - 10;
	float startX = (dx * (numRewards - 1)) / 2.0f;
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
