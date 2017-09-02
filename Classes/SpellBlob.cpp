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

bool SpellBlob::draggingAllowed = true;

bool SpellBlob::init(Spell *spell, bool draggable,
	std::function<void(int, Spell*)> onSelect,
	std::function<bool(Spell*)> isBeingUsed) {
	if ( !Layer::init() ) {
		return false;
	}
	this->setCascadeOpacityEnabled(true);
	
	this->spell = spell;
	this->draggable = draggable;
	this->dragging = false;
	this->isBeingUsed = isBeingUsed;

	auto bg = LoadSprite("ui/spellbox.png");
	bg->setAnchorPoint(Vec2(0.5, 0.5));
	bg->setPosition(Vec2(0, 0));
	this->addChild(bg);
	setContentSize(bg->getContentSize());
	
	auto label = Label::createWithTTF( spell->getName(), Fonts::TEXT_FONT, Fonts::SMALL_SIZE);
	label->setColor(Color3B::WHITE);
	label->setPosition(Vec2(0, -getContentSize().height/2-8));
	this->addChild(label, 1);
	
	mininode_grey = spell->makeNode(true);
	mininode_grey->setCascadeOpacityEnabled(true);
	mininode_grey->setOpacity(125);
	mininode_grey->setPosition(Vec2::ZERO);
	addChild(mininode_grey, 2);
	
	mininode = spell->makeNode(true);
	mininode->setPosition(Vec2::ZERO);
	mininode->setCascadeOpacityEnabled(true);
	addChild(mininode, 3);
	
	refresh();
	
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
			if (! this->draggable || ! draggingAllowed) {
				GameController::get()->showSpellInfoDialog(spell);
			} else {
				// Can only drag is the main mininode is visible
				this->dragging = mininode->isVisible();
			}
			return true;
		}
		
		return false; // if you are consuming it
	};
	if (draggable) {
		onClick->onTouchMoved = [this](Touch* touch, Event* event) -> bool {
			if (this->dragging) {
				auto offset = this->getParent()->getPosition() + getPosition();
				mininode->setPosition(touch->getLocation() - offset);
				mininode_grey->setVisible(true);
			}
			distanceMoved += touch->getDelta().length();
			// move mini-node to new location
			return false; // if you are consuming it
		};
		onClick->onTouchEnded = [this, onSelect](Touch* touch, Event* event) -> bool {
			auto bounds = event->getCurrentTarget()->getBoundingBox();
			bounds.origin -= bounds.size/2;
			bounds.origin += this->getParent()->getPosition();
			
			bool playSound = true;
			bool doSnap = true;
			if (bounds.containsPoint(touch->getLocation())){
				// If haven't moved far, then do this:
				if (distanceMoved < 2.0f) {
					playSound = false;
					if (draggingAllowed) {
						GameController::get()->showSpellInfoDialog(this->spell);
					}
				}
			} else if (this->dragging) {
				// if it's over a inventory spot, put the gem there
				layout_t layout = Game::get()->getLayout();
				const auto size = getContentSize();
				const float starty = layout.column_height - 110 * layout.ui_scale;
				for (int i = 0; i < 6; i++) {
					float originX = 18 * layout.ui_scale;
					if (i >= 3)
						originX = Game::get()->getBoundingBox().size.width - originX;
					originX -= size.width/2;
					
					auto bounds = Rect(
						Vec2(
							originX,
							starty - (i % 3) * 55 * layout.ui_scale - size.height/2),
						size
					);
					if (bounds.containsPoint(touch->getLocation())) {
						mininode->setPosition(Vec2::ZERO);
						mininode->setVisible(false);
						if (onSelect) {
							onSelect(i, this->spell);
						}
						doSnap = false;
					}
				}
			}
			if (this->dragging) {
				if (doSnap) {
					// Snap back to original position.
					if (playSound) {
						SoundManager::get()->playEffect(kSoundEffect_UISelectMinor);
					}
					auto snap = MoveTo::create(0.1f, Vec2::ZERO);
					auto hide = CallFunc::create([this]() {
						mininode_grey->setVisible(false);
					});
					mininode->stopAllActions();
					mininode->runAction(Sequence::create(snap, hide, nullptr));
				} else {
					SoundManager::get()->playEffect(kSoundEffect_UISelect);
				}
			}
			
			return true; // if you are consuming it
		};
	}
	_eventDispatcher->addEventListenerWithSceneGraphPriority(onClick, this);
	return true;
}

void SpellBlob::refresh() {
	bool beingUsed;
	if (! isBeingUsed) {
		beingUsed = false;
	} else {
		beingUsed = isBeingUsed(spell);
	}
	mininode->setVisible(! beingUsed);
	mininode_grey->setVisible(beingUsed);
}
