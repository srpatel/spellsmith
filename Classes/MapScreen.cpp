//
//  MapScreen.cpp
//  Gems
//
//  Created by Sunil Patel on 16/01/2017.
//
//

#include "Level.hpp"
#include "MapScreen.hpp"
#include "ui/CocosGUI.h"
#include "Strings.hpp"
#include "Map.hpp"
#include "Constants.h"
#include "GameController.hpp"
#include "NavigationBar.hpp"
#include "SaveData.hpp"
#include "SoundManager.hpp"

bool MapScreen::init() {
	if ( !Layer::init() ) {
		return false;
	}
	
	auto size = Director::getInstance()->getVisibleSize();
	
	// Add map and scroll blobs
	auto top = LoadSprite("map/scroll1.png");
	auto bot = LoadSprite("map/scroll1.png");
	
	auto topY = size.height - top->getContentSize().height / 2 - 10;
	auto bottomY = NavigationBar::HEIGHT + top->getContentSize().height / 2 + 10;
	
	// must multiply by scale factor because we load the
	// same resource in all resolutions
	auto map = MapScroll::create();
	
	auto buffer = 50;
	auto mapMaxY = bottomY + map->getContentSize().height/2;
	auto mapPreferredMaxY = mapMaxY;
	auto mapMinY = mapMaxY - (map->getContentSize().height - (topY - bottomY)) - top->getContentSize().height/2 + 5;
	auto mapPreferredMinY = mapMinY;
	
	auto clippedSize = Rect(Point::ZERO, Size(482/2, topY - bottomY));
	auto clippedOrigin = Vec2(getContentSize().width, topY + bottomY)/2;
	
	Sprite *stencil = Sprite::create();
	stencil->setTextureRect(clippedSize);
	stencil->setPosition(clippedOrigin);

	auto clipper = ClippingNode::create(stencil);
	clipper->setContentSize(getContentSize());
	clipper->addChild(map);
	
	auto onclick = EventListenerTouchOneByOne::create();
	onclick->setSwallowTouches(true);
	onclick->onTouchBegan = [this, clippedSize, clippedOrigin](Touch* touch, Event* event) -> bool {
		//auto map = event->getCurrentTarget();
		auto bounds = clippedSize;
		bounds.origin = clippedOrigin; // find the correct values
		bounds.origin -= bounds.size/2;
		
		if (bounds.containsPoint(touch->getLocation())){
			return true;
		}
		
		return false; // if you are consuming it
	};
	onclick->onTouchMoved = [this, map, mapPreferredMaxY, mapMaxY, mapPreferredMinY, mapMinY, top, bot](Touch* touch, Event* event) -> bool {
		auto dy = touch->getDelta().y;
		auto cy = map->getPositionY();
		auto newY = cy + dy;
		// if we're already above/below preferred, then minimise dy
		// minimise by more, the further away from preferred limit
		/*if (newY > mapPreferredMaxY && dy > 0) {
			auto perc = (newY - mapPreferredMaxY) / (mapMaxY - mapPreferredMaxY);
			dy *= (1 - perc) * (1 - perc);
		} else if (newY < mapPreferredMinY && dy < 0) {
			auto perc = (newY - mapPreferredMinY) / (mapMinY - mapPreferredMinY);
			dy *= (1 - perc) * (1 - perc);
		}*/
		newY = cy + dy;
		if (newY > mapMaxY) {
			newY = mapMaxY;
		} else if (newY < mapMinY) {
			newY = mapMinY;
		}
		map->setPositionY(newY);
		
		// Work out the scroll numbers
		auto step = 2.0f;
		int botIndex = 1 + ((int) (newY / step)) % 4;
		int topIndex = 5 - botIndex;
		
		char frameName[20];
		sprintf(frameName, "map/scroll%d.png", topIndex);
		top->setSpriteFrame(LoadSpriteFrame(frameName));
		sprintf(frameName, "map/scroll%d.png", botIndex);
		bot->setSpriteFrame(LoadSpriteFrame(frameName));
		return false; // if you are consuming it
	};
	// when you let go it should:
	//	- keep going in the same direction a bit
	//	- settle back to preferred bounds
	_eventDispatcher->addEventListenerWithSceneGraphPriority(onclick, clipper);

	map->setPosition(size.width/2, mapPreferredMaxY);
	top->setPosition(Vec2(
		size.width / 2,
		topY));
	bot->setPosition(Vec2(
		size.width / 2,
		bottomY));
	
	addChild(clipper);
	addChild(top);
	addChild(bot);
	
	/*
	// If you click on a level, go to that level.
	// If you click back, go to main menu.
	// If you click on spell book, go to spell book.
	auto button = ui::Button::create("ui/button_up.png", "ui/button_down.png", "ui/button_down.png", TEXTURE_TYPE);
	button->setTitleFontName(Fonts::TEXT_FONT);
	button->setTitleFontSize(Fonts::TEXT_SIZE);
	button->setPosition(Vec2(size.width/2, size.height/2));
	button->setTitleText _("ui.ARENA");
	button->addTouchEventListener([](Ref* pSender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED) {
			// For now, from scratch always!
			PLAY_SOUND( kSoundEffect_UISelect );
			GameController::get()->startArena();
		}
	});
	this->addChild(button);
	*/
	// todo: coordinates are such on the original image
	// scale map so that width is correct, and then scale offsets by that amount too
	nodes = Layer::create();
	nodes->setVisible(false);
	addChild(nodes);
	
	return true;
}

void MapScreen::refreshNodes() {
	return;
	nodes->removeAllChildren();
	for (EventListener *l : listeners) {
		_eventDispatcher->removeEventListener(l);
	}
	listeners.clear();
	
	float offset_y = 50;
	for (RoundDef * r : LevelManager::get()->getRoundDefinitions()) {
		// Make a node and add it!
		auto n =
			SaveData::isLevelComplete(r->name) ?
			LoadSprite("icons/mapnodedone.png") :
			LoadSprite("icons/mapnode.png");
		n->setAnchorPoint(Vec2(0.5, 0.5));
		n->setPosition(r->x, r->y + offset_y);
		nodes->addChild(n);
		
		auto t = Label::createWithTTF(r->name, Fonts::TEXT_FONT, Fonts::TEXT_SIZE);
		t->setTextColor(Color4B::WHITE);
		t->setPosition(n->getPosition());
		nodes->addChild(t);
		
		auto onclick = EventListenerTouchOneByOne::create();
		onclick->setSwallowTouches(true);
		onclick->onTouchBegan = [this, r](Touch* touch, Event* event) -> bool {
			auto bounds = event->getCurrentTarget()->getBoundingBox();
			
			if (bounds.containsPoint(touch->getLocation())){
				PLAY_SOUND( kSoundEffect_UISelect );
				GameController::get()->showPreLevelDialog(r);
				return true;
			}
			
			return false; // if you are consuming it
		};
		_eventDispatcher->addEventListenerWithSceneGraphPriority(onclick, n);
		listeners.push_back(onclick);
	}
}
