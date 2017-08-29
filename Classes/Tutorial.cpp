//
//  Tutorial.cpp
//  Gems
//
//  Created by Sunil Patel on 27/08/2017.
//
//

#include "Tutorial.hpp"
#include "SaveData.hpp"
#include "GameScene.hpp"
#include "Spellbook.hpp"
#include "SpellBlob.hpp"
#include "GameController.hpp"
#include "Popup.hpp"
#include "Strings.hpp"
#include "Constants.h"

#define kPosGrid 1
#define kPosScenery 2

int currentLevel = -1;
Layer *currentPopup = nullptr;
std::list<Node *> others;

Layer *makeTextBox(std::string text, int location, bool withTtc, float delay) {
	EventDispatcher *ed = Director::getInstance()->getEventDispatcher();
	auto game = Game::get();
	if (currentPopup != nullptr) {
		currentPopup->removeFromParent();
	}

	auto yBuffer = 50;
	auto size = Size(200, 0);
	
	// Add wrapping text in the middle...
	auto label = Label::createWithTTF(text, Fonts::TEXT_FONT, Fonts::TEXT_SIZE);
	label->setDimensions(size.width - 20, 0);
	label->setAlignment(TextHAlignment::CENTER);
	size.height = label->getContentSize().height + yBuffer;
	// position the label on the center of the screen
	label->setPosition(size/2 + Size(0, withTtc ? 15 : 0));
	
	auto popup = Popup::create(size.width, size.height);
	currentPopup = popup;
	popup->setCascadeOpacityEnabled(true);
	popup->addChild(label);
	
	if (withTtc) {
		// tap to continue label
		auto ttc = Label::createWithTTF(_("ui.TAP_TO_CONTINUE"), Fonts::TEXT_FONT, Fonts::SMALL_SIZE);
		ttc->setDimensions(size.width - 20, 0);
		ttc->setAlignment(TextHAlignment::CENTER);
		// position the label on the center of the screen
		ttc->setPosition(Vec2(size.width/2, 20));
		ttc->setOpacity(0);
		auto fadeIn = EaseIn::create(FadeIn::create(1), 0.5f);
		auto fadeOut = EaseOut::create(FadeOut::create(1), 0.5f);
		auto sequence = RepeatForever::create(Sequence::create(fadeIn, fadeOut, DelayTime::create(0.5f), nullptr));
		ttc->runAction(sequence);
		popup->addChild(ttc);
	}
	
	Vec2 position;
	
	if (location == kPosGrid) {
		auto gridCentre = game->grid->getPosition();
		position = gridCentre - size/2;
	} else if (location == kPosScenery) {
		auto sceneCentre = game->scenery->getPosition();
		auto sceneSize = game->scenery->getContentSize();
		position = sceneCentre + sceneSize/2 - size/2;
	}
	popup->setPosition(position);
	
	popup->setOpacity(0);
	auto fadeIn = Sequence::create(DelayTime::create(delay), FadeIn::create(0.5), nullptr);
	popup->runAction(fadeIn);
	
	// On tap, fade out and advance tutorial...
	if (withTtc) {
		auto touchListener = cocos2d::EventListenerTouchOneByOne::create();
		touchListener->setSwallowTouches(true);
		touchListener->onTouchBegan = [game, popup, size, position, ed, touchListener](Touch *touch, Event *event) {
			auto bounds = Rect(
				position,
				size
			);
			if (bounds.containsPoint(touch->getLocation())) {
				event->stopPropagation();
				Tutorial::activate(currentLevel + 1);
				// remove this listener...
				ed->removeEventListener(touchListener);
				return true;
			}
			return false;
		};
		ed->addEventListenerWithSceneGraphPriority(touchListener, popup);
	}
	
	return popup;
}

void Tutorial::cancelAll() {
	currentLevel = -1;
	if (currentPopup != nullptr) {
		currentPopup->removeFromParent();
		currentPopup = nullptr;
	}
}

int Tutorial::getCurrent() {
	return currentLevel;
}

void Tutorial::activate(int number) {
	auto game = Game::get();
	// Starting numbers: 1, 7, 101
	bool start = number == 1 || number == 7 || number == 101 || number == 201;
	bool prec = number == currentLevel + 1;
	
	// Don't progress to the next tutorial unless you're starting a chain
	// or are moving to the next tutorial.
	if (! start && ! prec)
		return;

	// Popups which stay visible twice: 3 (not anymore)
	auto remover = Sequence::create(FadeOut::create(1), RemoveSelf::create(), nullptr);
	if (currentPopup != nullptr /*&& number != 3*/) {
		currentPopup->stopAllActions();
		// Sometimes we don't want to fade it out (when changing screens)
		if (number == 11) {
			currentPopup->removeFromParent();
		} else {
			currentPopup->runAction(remover->clone());
		}
		currentPopup = nullptr;
	}
	if (! others.empty()) {
		for (Node *n : others) {
			n->stopAllActions();
			n->runAction(remover->clone());
		}
		others.clear();
	}
	
	currentLevel = number;
	
	// Tutorial for level 1.
	if (number < 100) {
		if (number == 1) {
			// Start down this path!
			// Grid fakeout -- pre-determine grid.
			game->grid->preset(1);
			
			// Cannot use grid
			game->grid->setActive(false);
			
			// Make text box over grid...
			auto popup = makeTextBox(
				_("tutorial.1"),
				kPosGrid, true, 0);
			game->addChild(popup);
		} else if (number == 2) {
			game->grid->setActive(true);
			game->grid->flashPreset(1);
			// Fade out everything except the path we want to suggest
			
			auto popup = makeTextBox(
				_("tutorial.2"),
				kPosScenery, false, 0.5);
			game->addChild(popup);
		} else if (number == 3) {
			// Fade everything in...
			game->grid->flashPreset(0);
		} else if (number == 4) {
		} else if (number == 5) {
			game->grid->setActive(false);
			auto popup = makeTextBox(
				_("tutorial.5"),
				kPosGrid, true, 0);
			game->addChild(popup);
			// point to red circle
		} else if (number == 6) {
			game->grid->setActive(true);
		} else if (number == 7) {
			// If our spellbook contains only one spell, and there are no
			// spells equipped, we do this.
			bool spellEquipped = false;
			For (6) {
				std::string name = SaveData::getEquippedSpellAt(i);
				if (! name.empty()) {
					spellEquipped = true;
					break;
				}
			}
			bool learntFireball = false;
			bool learntOneOnly = true;
			for (auto s : SaveData::getSpells()) {
				if (s == "fireball") {
					learntFireball = true;
				} else {
					learntOneOnly = false;
				}
			}
			
			if (spellEquipped || ! learntFireball || ! learntOneOnly) {
				// No need to do tut.
				currentLevel = -1;
				return;
			}
			
			auto spellbook = (Spellbook *) (GameController::get()->getScreen(kStateSpellbook));
			
			// Disable navigation
			//GameController::get()->enableBar(false);
			GameController::get()->showButton(false);
			
			// Disallow dragging spells
			SpellBlob::draggingAllowed = false;
			
			// Learn spell text
			auto popup = makeTextBox(
				_("tutorial.7"),
				kPosScenery, false, 0.5);
			spellbook->addChild(popup);
			
			// Flash new spell
			/*auto blob = spellbook->blobs[0];
			auto fadeIn = EaseIn::create(FadeIn::create(1), 0.5f);
			auto fadeOut = EaseOut::create(FadeOut::create(1), 0.5f);
			auto sequence = RepeatForever::create(Sequence::create(fadeIn, fadeOut, DelayTime::create(0.5f), nullptr));
			blob->runAction(sequence);*/
			// ???
		} else if (number == 8) {
			//auto spellbook = (Spellbook *) (GameController::get()->getScreen(kStateSpellbook));
			//auto blob = spellbook->blobs[0];
			//blob->stopAllActions();
		} else if (number == 9) {
			auto spellbook = (Spellbook *) (GameController::get()->getScreen(kStateSpellbook));
			auto popup = makeTextBox(
				_("tutorial.9"),
				kPosScenery, false, 0.5);
			spellbook->addChild(popup);
			
			// Show arrow
			auto finger = LoadSprite("ui/fingerpoint.png");
			finger->setAnchorPoint(Vec2(0.5, 1));
			auto blob = spellbook->blobs[0];
			layout_t layout = Game::get()->getLayout();
			auto fromPos = blob->getPosition() + blob->getParent()->getPosition();
			auto toPos = Vec2(18 * layout.ui_scale, layout.column_height - 110 * layout.ui_scale);
			spellbook->addChild(finger, 100);
			finger->setOpacity(0);
			finger->runAction(RepeatForever::create(
				Sequence::create(
					MoveTo::create(0, fromPos),
					FadeIn::create(0.2f),
					DelayTime::create(0.2f),
					MoveTo::create(0.8f, toPos),
					DelayTime::create(0.2f),
					FadeOut::create(0.2f),
					nullptr
				)
			));
			others.push_back(finger);
			
			// Re-allow dragging spells
			SpellBlob::draggingAllowed = true;
		} else if (number == 10) {
			auto spellbook = (Spellbook *) (GameController::get()->getScreen(kStateSpellbook));
			auto popup = makeTextBox(
				_("tutorial.10"),
				kPosScenery, false, 1);
			spellbook->addChild(popup, 100);
			
			// Re-enable ui
			//GameController::get()->enableBar(true);
			GameController::get()->showButton(true);
		} else if (number == 11) {
		}
	} else
	// Tutorial for level 2
	if (number < 200) {
		if (number == 101) {
			// Only allow if there is one spell equipped -- fireball.
			bool fireball = false;
			bool nonFireball = false;
			For (6) {
				std::string name = SaveData::getEquippedSpellAt(i);
				if (! name.empty()) {
					if (name == "fireball") {
						fireball = true;
					} else {
						nonFireball = true;
					}
				}
			}
			
			if (! fireball || nonFireball) {
				// No need to do tut.
				currentLevel = -1;
				return;
			}
			
			// Start down this path!
			
			// Grid fakeout -- pre-determine grid.
			game->grid->preset(2);
			
			// Cannot use grid
			game->grid->setActive(false);
			
			// Make text box over grid...
			auto popup = makeTextBox(
				_("tutorial.101"),
				kPosGrid, true, 0);
			game->addChild(popup);
		} else if (number == 102) {
			game->grid->setActive(true);
			game->grid->flashPreset(2);
			// Make text box over grid...
			auto popup = makeTextBox(
				_("tutorial.102"),
				kPosScenery, false, 0);
			game->addChild(popup);
		} else if (number == 103) {
			game->grid->flashPreset(0);
			// Make text box over grid...
			auto popup = makeTextBox(
				_("tutorial.103"),
				kPosScenery, true, 0.5);
			game->addChild(popup);
		} else if (number == 104) {
		}
	} else
	// Tutorial for 4th spell
	if (number < 300) {
		if (number == 201) {
			// If our spellbook contains >=4 spells, and there are only
			// spells equipped in the left column.
			bool rightColumn = false;
			for (int i = 3; i < 6; i++) {
				std::string name = SaveData::getEquippedSpellAt(i);
				if (! name.empty()) {
					rightColumn = true;
					break;
				}
			}
			
			if (rightColumn || SaveData::getSpells().size() < 4) {
				// No need to do tut.
				currentLevel = -1;
				return;
			}
			
			auto spellbook = (Spellbook *) (GameController::get()->getScreen(kStateSpellbook));
			
			// Disable navigation
			//GameController::get()->enableBar(false);
			GameController::get()->showButton(false);
			
			// Learn spell text
			auto popup = makeTextBox(
				_("tutorial.201"),
				kPosScenery, false, 0.5);
			spellbook->addChild(popup);
			
			// Show arrow
			auto finger = LoadSprite("ui/fingerpoint.png");
			finger->setAnchorPoint(Vec2(0.5, 1));
			SpellBlob *blob = nullptr;
			for (SpellBlob *sb : spellbook->blobs) {
				if (sb->mininode->isVisible()) {
					blob = sb;
					break;
				}
			}
			if (blob != nullptr) {
				layout_t layout = Game::get()->getLayout();
				auto fromPos = blob->getPosition() + blob->getParent()->getPosition();
				auto toPos = Vec2(Game::get()->getBoundingBox().size.width - 18 * layout.ui_scale, layout.column_height - 110 * layout.ui_scale);
				spellbook->addChild(finger, 100);
				finger->setOpacity(0);
				finger->runAction(RepeatForever::create(
					Sequence::create(
						MoveTo::create(0, fromPos),
						FadeIn::create(0.2f),
						DelayTime::create(0.2f),
						MoveTo::create(0.8f, toPos),
						DelayTime::create(0.2f),
						FadeOut::create(0.2f),
						nullptr
					)
				));
				others.push_back(finger);
			}
		} else if (number == 202) {
			GameController::get()->showButton(true);
		}
	}
}
