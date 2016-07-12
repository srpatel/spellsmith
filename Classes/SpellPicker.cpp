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
#include "GameScene.hpp"

class SpellBlob : public Layer {
public:
	bool init(Spell *);
	CREATE_FUNC_1(SpellBlob, Spell *);
private:
};

bool SpellBlob::init(Spell *spell) {
	if ( !Layer::init() ) {
		return false;
	}
	
	auto bg = LoadSprite("ui/spellbox.png");
	bg->setAnchorPoint(Vec2(0.5, 0.5));
	bg->setPosition(Vec2(0, 0));
	this->addChild(bg);
	setContentSize(bg->getContentSize());
	
	auto label = Label::createWithTTF( spell->getName(), Fonts::TEXT_FONT, Fonts::SMALL_SIZE);
	label->setColor(Color3B::WHITE);
	label->setPosition(Vec2(0, -bg->getContentSize().height/2-5));
	this->addChild(label, 1);
	
	// TODO - need to make sure we don't add this in two places at once?
	auto mininode = spell->mininode;
	addChild(mininode);
	
	auto onClick = EventListenerTouchOneByOne::create();
	onClick->setSwallowTouches(true);
	onClick->onTouchBegan = [this](Touch* touch, Event* event) -> bool {
		auto bounds = event->getCurrentTarget()->getBoundingBox();
		bounds.origin -= bounds.size/2;
		bounds.origin += this->getParent()->getPosition();
		
		if (bounds.containsPoint(touch->getLocation())){
			//GameController::get()->showSpellInfoDialog(spell);
			return true;
		}
		
		return false; // if you are consuming it
	};
	onClick->onTouchMoved = [this, mininode](Touch* touch, Event* event) -> bool {
		auto offset = this->getParent()->getPosition() + getPosition();
		mininode->setPosition(touch->getLocation() - offset);
		// move mini-node to new location
		return false; // if you are consuming it
	};
	onClick->onTouchEnded = [this, bg, mininode, spell](Touch* touch, Event* event) -> bool {
		auto bounds = event->getCurrentTarget()->getBoundingBox();
		bounds.origin -= bounds.size/2;
		bounds.origin += this->getParent()->getPosition();
		
		bool doSnap = true;
		if (bounds.containsPoint(touch->getLocation())){
			// If haven't moved far, then do this:
			GameController::get()->showSpellInfoDialog(spell);
		} else {
			// if it's over a inventory spot, put the gem there
			layout_t layout = Game::get()->getLayout();
			const auto size = bg->getContentSize();
			const float starty = layout.column_height - 110;
			for (int i = 0; i < 3; i++) {
				auto bounds = Rect(
					Vec2(18 - size.width/2, starty - i * 55 - size.height/2),
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
								   Vec2(Game::get()->getBoundingBox().size.width - 18 - size.width/2, starty - i * 55 - size.height/2),
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
	_eventDispatcher->addEventListenerWithSceneGraphPriority(onClick, this);
	
	return true;
}

bool SpellPicker::init(Spell *s1, Spell *s2) {
	if ( !Layer::init() ) {
		return false;
	}
	
	auto bg = LoadSprite("ui/popup.png");
	bg->setAnchorPoint(Vec2(0.5, 0.5));
	bg->setPosition(Vec2(0, 0));
	this->addChild(bg);
	
	// "Pick a spell" title
	auto label = Label::createWithTTF( "choose a spell", Fonts::TITLE_FONT, Fonts::TEXT_SIZE);
	label->setColor(Color3B::WHITE);
	label->setPosition(Vec2(0, 50));
	addChild(label, 1);
	
	auto sb1 = SpellBlob::create(s1);
	sb1->setPosition(5-bg->getContentSize().width/4, 0);
	addChild(sb1);
	
	auto sb2 = SpellBlob::create(s2);
	sb2->setPosition(-5+bg->getContentSize().width/4, 0);
	addChild(sb2);
	
	/*auto skip = Label::createWithTTF( "skip", Fonts::TEXT_FONT, Fonts::SMALL_SIZE);
	skip->setColor(Color3B::WHITE);
	skip->setPosition(Vec2(0, -bg->getContentSize().height/2+20));
	addChild(skip, 1);*/
	
	return true;
}