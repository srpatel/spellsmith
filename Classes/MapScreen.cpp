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

void MapScreen::onSelect() {
	refreshNodes();
}

bool MapScreen::init() {
	if ( !Layer::init() ) {
		return false;
	}
	
	auto size = Director::getInstance()->getVisibleSize();
	
	auto title = LoadSprite("main/logo.png");
	title->setScale(0.5);
	title->setAnchorPoint(Vec2(0.5, 0.5));
	Vec2 pos(size.width/2, size.height - title->getContentSize().height/4 - 20);
	title->setPosition(pos);
	this->addChild(title);
	
	// Add map and scroll blobs
	auto top = LoadSprite("map/scroll1.png");
	auto bot = LoadSprite("map/scroll1.png");
	
	auto topY = pos.y - title->getContentSize().height/4 - top->getContentSize().height / 2 - 10;
	auto bottomY = NavigationBar::HEIGHT + top->getContentSize().height / 2 + 10;
	
	// must multiply by scale factor because we load the
	// same resource in all resolutions
	map = MapScroll::create();
	
	auto buffer = 50;
	mapMaxY = bottomY + map->getContentSize().height/2;
	auto mapPreferredMaxY = mapMaxY;
	mapMinY = mapMaxY - (map->getContentSize().height - (topY - bottomY)) - top->getContentSize().height/2 + 5;
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
			// are we on a node?
			currentRound = nullptr;
			for (auto n : map->nodes) {
				auto round = n.first;
				auto sprite = n.second;
				auto theBounds = sprite->getBoundingBox();
				theBounds.origin += map->getPosition();
				if (!round->depends.empty() && ! SaveData::isLevelComplete(round->depends)) {
					continue;
				}
				
				if (theBounds.containsPoint(touch->getLocation())){
					// don't stop propagation -- we want the map to capture too...
					currentRound = round;
					break;
				}
			}
			return true;
		}
		
		return false; // if you are consuming it
	};
	onclick->onTouchMoved = [this, mapPreferredMaxY, mapPreferredMinY, top, bot](Touch* touch, Event* event) -> bool {
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
		return true; // if you are consuming it
	};
	onclick->onTouchEnded = [this, mapPreferredMaxY, mapPreferredMinY, top, bot](Touch* touch, Event* event) -> bool {
		auto dx = touch->getLocation().distance(touch->getStartLocation());
		if (dx < kMaxTapDistance) {
			if (currentRound != nullptr) {
				PLAY_SOUND( kSoundEffect_UISelect );
				GameController::get()->showPreLevelDialog(currentRound);
				currentRound = nullptr;
			}
			return true;
		}
		return false;
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
	
	return true;
}

void MapScreen::scrollToUncomplete() {
	Node *lowestUncomplete = nullptr;
	for (auto p : map->nodes) {
		bool completed =  SaveData::isLevelComplete(p.first->name);
		if (! completed) {
			if (lowestUncomplete == nullptr
				|| p.second->getPositionY() < lowestUncomplete->getPositionY()) {
				lowestUncomplete = p.second;
			}
		}
	}
	if (lowestUncomplete != nullptr) {
		// Scroll to this position!
		float current = map->getPositionY();
		float target = mapMaxY
			- (lowestUncomplete->getPositionY() + map->getContentSize().height/2) // node distance from bottom of map
			+ map->getContentSize().height / 2; // aim for it to be 1/3 from the top
		if (current > target) {
			if (target > mapMaxY) target = mapMaxY;
			if (target < mapMinY) target = mapMinY;
			map->setPositionY(target);
		}
	}
}

void MapScreen::refreshNodes() {
	map->refreshNodes();
	return;
}
