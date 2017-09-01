//
//  Spellbook.cpp
//  Gems
//
//  Created by Sunil Patel on 18/01/2017.
//
//

#include "Spellbook.hpp"
#include "Spell.hpp"
#include "SpellBlob.hpp"
#include "NavigationBar.hpp"
#include "GameScene.hpp"
#include "GameController.hpp"
#include "SoundManager.hpp"
#include "SaveData.hpp"
#include "Tutorial.hpp"
#include "Constants.h"

static layout_t layout;

void Spellbook::onSelect() {
	ColumnScreen::onSelect();
	refreshEquips();
	refreshSpells();
}

bool Spellbook::init() {
	if ( !ColumnScreen::init() ) {
		return false;
	}
	
	setBackground("bg_lab1.png");
		
	spell_holder = Layer::create();
	spell_holder->setPosition(layout.column_width, NavigationBar::HEIGHT);
	addChild(spell_holder, 7);
	
	equipped_holder = Layer::create();
	addChild(equipped_holder, 6);
	
	auto grad = LayerColor::create();
	grad->initWithColor(Color4B(95, 91, 85, 255));
	grad->setPosition(Vec2(6, 340) * layout.ui_scale);
	grad->setContentSize(Size(27, 30) * layout.ui_scale);
	this->addChild(grad, 6);
	
	// Pages
	{
		left = LoadSprite("ui/buttonback_alpha.png");
		right = LoadSprite("ui/buttonback_alpha.png");
		left->setAnchorPoint(Vec2(0.5, 0.5));
		right->setAnchorPoint(Vec2(0.5, 0.5));
		right->setScaleX(-1);
		auto y = layout.column_height - layout.bar_top_height / 2 - 5 * layout.ui_scale;
		left->setPosition(layout.column_width + 20, y);
		right->setPosition(getBoundingBox().size.width - layout.column_width - 20, y);
		
		// label in the middle
		page_label = Label::createWithTTF( "1/1", Fonts::NUMBER_FONT, Fonts::TEXT_SIZE);
		page_label->setPosition(getBoundingBox().size.width / 2, y);
		addChild(page_label, 1);
		
		addChild(left, 1);
		addChild(right, 1);
		
		auto onArrowClick = EventListenerTouchOneByOne::create();
		onArrowClick->setSwallowTouches(true);
		// trigger when you push down
		onArrowClick->onTouchBegan = [this](Touch* touch, Event* event) -> bool {
			auto target = event->getCurrentTarget();
			auto bounds = target->getBoundingBox();
			
			if (bounds.containsPoint(touch->getLocation())){
				if (target == left) {
					if (current_page > 0) {
						PLAY_SOUND(kSoundEffect_UISelectMinor);
						current_page--;
						refreshSpells();
					}
				} else {
					if (current_page < num_pages - 1) {
						PLAY_SOUND(kSoundEffect_UISelectMinor);
						current_page++;
						refreshSpells();
					}
				}
				return true;
			}
			
			return false; // if you are consuming it
		};
		_eventDispatcher->addEventListenerWithSceneGraphPriority(onArrowClick, left);
		_eventDispatcher->addEventListenerWithSceneGraphPriority(onArrowClick->clone(), right);
	}
	
	current_page = 0;
	num_pages = 1;
	
	return true;
}

void Spellbook::refreshEquips() {
	const float starty = layout.column_height - 110 * layout.ui_scale;
	
	equipped_holder->removeAllChildren();
	for (EventListener *l : equipped_listeners)
		_eventDispatcher->removeEventListener(l);
	equipped_listeners.clear();
	
	for (int i = 0; i < 6; i++) {
		std::string name = SaveData::getEquippedSpellAt(i);
		if (! name.empty()) {
			Spell *spell = SpellManager::get()->getByName(name);
			if (spell != nullptr) {
				auto y = starty - (i % 3) * 55 * layout.ui_scale;
				auto x = (i < 3) ?
					18 * layout.ui_scale :
					getBoundingBox().size.width - 18 * layout.ui_scale;
			
				// Equip the spell!
				// TODO - ensure we can share the mininode between spellbook/game
				auto sprite = spell->makeNode(true);
				sprite->setPosition(x, y);
				auto onSpellClick = EventListenerTouchOneByOne::create();
				onSpellClick->setSwallowTouches(true);
				// trigger when you push down
				onSpellClick->onTouchBegan = [this, spell](Touch* touch, Event* event) -> bool {
					auto bounds = event->getCurrentTarget()->getBoundingBox();
					bounds.origin -= bounds.size/2;
					
					if (bounds.containsPoint(touch->getLocation())){
						PLAY_SOUND(kSoundEffect_UISelectMinor);
						GameController::get()->showSpellInfoDialog(spell);
						return true;
					}
					
					return false; // if you are consuming it
				};
				// We need to add this to a list and delete ourselves because the sprite's destructor is never called.
				// It's the shared mininode instance.
				_eventDispatcher->addEventListenerWithSceneGraphPriority(onSpellClick, sprite);
				equipped_listeners.push_back(onSpellClick);
				equipped_holder->addChild(sprite);
			}
		}
	}
	
	for (SpellBlob *b : blobs)
		b->refresh();
}

void Spellbook::refreshSpells() {
	spell_holder->removeAllChildren();
	blobs.clear();
	int numberPerRow = 3;
	float dy = 65;
	float dx = (getBoundingBox().size.width - 2 * layout.column_width) / numberPerRow;
	
	float totalHeight = layout.column_height - layout.bar_top_height - NavigationBar::HEIGHT;
	
	int numRows = (int) (totalHeight / dy);
	int numPerPage = numRows * numberPerRow;
	
	auto spells = SaveData::getSpells();
	
	// Adjust dy to be exact
	dy = totalHeight / numRows;
	
	// Fix num pages...
	num_pages = ceil((float) spells.size() / numPerPage);
	page_label->setString(ToString(current_page + 1) + std::string("/") + ToString(num_pages));
	bool paginationVisible = num_pages > 1;
	left->setVisible(paginationVisible);
	right->setVisible(paginationVisible);
	page_label->setVisible(paginationVisible);
	left->setOpacity((current_page == 0) ? 125 : 255);
	right->setOpacity((current_page == num_pages - 1) ? 125 : 255);
	
	// Shift up by 8px because that's how far down the label is from the centre of the spellblob
	float startY = totalHeight + 8;
	for (int i = 0; i < numPerPage; i++) {
		int currentIndex = i + current_page * numPerPage;
		if (spells.size() <= currentIndex)
			break;
		std::string spellname = spells[currentIndex];
		Spell *spell = SpellManager::get()->getByName(spellname);
		
		float currentX = dx * (i % numberPerRow) + dx/2.0;
		float currentY = startY - dy * (int) (i / numberPerRow) - dy/2.0;
		
		auto onSelect = [this](int invPosition, Spell *selected){
			Tutorial::activate(10);
			Tutorial::activate(202);
			SaveData::setEquippedSpellAt(invPosition, selected->getRawName());
			refreshEquips();
		};
		
		auto isBeingUsed = [this](Spell *selected)->bool{
			For(6) {
				auto sn = SaveData::getEquippedSpellAt(i);
				if (sn == selected->getRawName()) {
					return true;
				}
			}
			return false;
		};
		
		auto sb = SpellBlob::create(spell, true, onSelect, isBeingUsed);
		sb->setPosition(currentX, currentY);
		blobs.push_back(sb);
		spell_holder->addChild(sb);
	}
}
