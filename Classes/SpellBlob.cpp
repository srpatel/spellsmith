//
//  SpellBlob.cpp
//  Gems
//
//  Created by Sunil Patel on 25/01/2017.
//
//

#include "SpellBlob.hpp"
#include "Constants.h"
#include "GameController.hpp"
#include "SoundManager.hpp"
#include "GameScene.hpp"

bool SpellBlob::init(Spell *spell, bool d) {
	if ( !Layer::init() ) {
		return false;
	}
	
	this->draggable = d;

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
	addChild(mininode, 2);
	
	auto onClick = EventListenerTouchOneByOne::create();
	onClick->setSwallowTouches(true);
	onClick->onTouchBegan = [this, spell](Touch* touch, Event* event) -> bool {
		// bring to front
		auto par = this->getParent();
		par->reorderChild(this, this->getLocalZOrder());
		
		distanceMoved = 0;
		
		auto bounds = event->getCurrentTarget()->getBoundingBox();
		bounds.origin -= bounds.size/2;
		bounds.origin += this->getParent()->getPosition();
		
		if (bounds.containsPoint(touch->getLocation())){
			// If it's not draggable, show the spell info dialog straight away.
			// Otherwise, wait for the release.
			SoundManager::get()->playEffect(kSoundEffect_UISelectMinor);
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
			
			bool playSound = true;
			bool doSnap = true;
			if (bounds.containsPoint(touch->getLocation())){
				// If haven't moved far, then do this:
				if (distanceMoved < 2.0f) {
					playSound = false;
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
				if (playSound) {
					SoundManager::get()->playEffect(kSoundEffect_UISelectMinor);
				}
				auto snap = MoveTo::create(0.1f, Vec2::ZERO);
				mininode->stopAllActions();
				mininode->runAction(snap);
			} else {
				SoundManager::get()->playEffect(kSoundEffect_UISelect);
				Game::get()->spellPicked();
				getParent()->removeFromParent();
			}
			
			return true; // if you are consuming it
		};
	}
	_eventDispatcher->addEventListenerWithSceneGraphPriority(onClick, this);
	return true;
}
