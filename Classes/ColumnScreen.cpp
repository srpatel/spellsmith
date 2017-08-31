//
//  ColumnScreen.cpp
//  Gems
//
//  Created by Sunil Patel on 31/08/2017.
//
//

#include "ColumnScreen.hpp"
#include "Constants.h"
#include "GameController.hpp"
#include "SoundManager.hpp"

/*layout_t ColumnScreen::getLayout() {
	return layout;
}*/

#define kDepthBarTop 1
#define kDepthBarBottom 1
#define kDepthColumns 5
#define kDepthSmokey 20
#define kDepthButton 10

void ColumnScreen::onSelect() {
	mute_button->setSpriteFrame(
		SoundManager::get()->getMute() ?
		"icons/speakercross.png" :
		"icons/speaker.png"
	);
}

void ColumnScreen::setMapButtonVisible(bool visible) {
	canClickMap = visible;
	map_button->stopAllActions();
	map_button->runAction(FadeTo::create(0.2, visible ? 255 : 0));
}

bool ColumnScreen::init() {
    if ( !Screen::init() ) {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
	setContentSize(visibleSize);
	
	right_col_sprite = LoadSprite("ui/column_right.png");
	
	
	layout.column_height = right_col_sprite->getBoundingBox().size.height;
	layout.column_width = right_col_sprite->getBoundingBox().size.width;
	
	// if the scenery is too small...
	float min_scenery_height = 105;
	float ui_scale = 1;
	float current_scenery_height = getBoundingBox().size.height - layout.column_height;
	if (current_scenery_height < min_scenery_height) {
		layout.column_height = getBoundingBox().size.height - min_scenery_height;
		ui_scale = layout.column_height / right_col_sprite->getBoundingBox().size.height;
		layout.column_width *= ui_scale;
	}
	layout.ui_scale = ui_scale;
/*
 _                _                                   _     
| |              | |                                 | |    
| |__   __ _  ___| | ____ _ _ __ ___  _   _ _ __   __| |___ 
| '_ \ / _` |/ __| |/ / _` | '__/ _ \| | | | '_ \ / _` / __|
| |_) | (_| | (__|   < (_| | | | (_) | |_| | | | | (_| \__ \
|_.__/ \__,_|\___|_|\_\__, |_|  \___/ \__,_|_| |_|\__,_|___/
                       __/ |                                
                      |___/
*/
	// Scenery
	{
		layout.scenery_height = getBoundingBox().size.height - layout.column_height;
		Size scenerySize(getBoundingBox().size.width, getBoundingBox().size.height - layout.column_height + 5);
		background = LoadLargeSprite("bg_graveyard1.png");
		background->setAnchorPoint(Vec2(0.5, 0.5));
		
		//Fit width:
		float targetWidth = scenerySize.width;
		float actualWidth = background->getBoundingBox().size.width;
		float wRatio = targetWidth/actualWidth;
		
		float targetHeight = scenerySize.height;
		float actualHeight = background->getBoundingBox().size.height;
		float hRatio = targetHeight/actualHeight;
		
		float ratio = MAX(hRatio, wRatio);
		background->setScale(ratio);
		
		//background->setPosition({visibleSize.height - ratio * scenerySize.height/2, ratio * scenerySize.width / 2});
		background->setPosition({visibleSize.width / 2, visibleSize.height - background->getBoundingBox().size.height/2});
		addChild(background, 0);
	}
	// Gem background
	{
		auto grad = LayerColor::create();
		grad->initWithColor(Color4B(Colours::GEM_BACKGROUND));
		grad->setPosition(Director::getInstance()->getVisibleOrigin());
		grad->setContentSize(Size(getBoundingBox().size.width, layout.column_height));
		this->addChild(grad);
	}
	// Bar top
	{
		bar_top = LoadSprite("ui/bar_top.png");
		layout.bar_top_height = bar_top->getBoundingBox().size.height * ui_scale;
		bar_top->setAnchorPoint(Vec2(0.5, 1));
		bar_top->setPosition(Vec2(getBoundingBox().size.width/2, layout.column_height));
		bar_top->setScale(ui_scale);
		this->addChild(bar_top, kDepthBarTop);
	}
	
	// bar bottom
	{
		auto sprite = LoadSprite("ui/bar_bottom.png");
		layout.bar_bottom_height = sprite->getBoundingBox().size.height * ui_scale;
		sprite->setAnchorPoint(Vec2(0.5, 0));
		sprite->setPosition(Vec2(getBoundingBox().size.width/2, 0));
		sprite->setScale(ui_scale);
		this->addChild(sprite, kDepthBarBottom);
	}
	
	// Columns
	{
		//auto sprite = LoadSprite("ui/column_right.png");
		right_col_sprite->setAnchorPoint(Vec2(1, 1));
		right_col_sprite->setPosition(Vec2(getBoundingBox().size.width, layout.column_height));
		right_col_sprite->setScale(ui_scale);
		this->addChild(right_col_sprite, kDepthColumns);
	}
	{
		auto sprite = LoadSprite("ui/column_left.png");
		sprite->setAnchorPoint(Vec2(0, 1));
		sprite->setPosition(Vec2(0, layout.column_height));
		sprite->setScale(ui_scale);
		this->addChild(sprite, kDepthColumns);
	}
	
	// Smokey
	{
		auto sprite = LoadLargeSprite("smokey.png");
		sprite->setAnchorPoint(Vec2(0.5, 1));
		sprite->setPosition(Vec2(getBoundingBox().size.width/2, layout.column_height));
		this->addChild(sprite, kDepthSmokey);
	}

	

/*   _ _       _
    | (_)     | |                
  __| |_  __ _| | ___   __ _ ___ 
 / _` | |/ _` | |/ _ \ / _` / __|
| (_| | | (_| | | (_) | (_| \__ \
 \__,_|_|\__,_|_|\___/ \__, |___/
                        __/ |    
                       |___/
 */
	map_button = LoadSprite("icons/map.png");
	map_button->setAnchorPoint(Vec2(0.5, 0.5));
	map_button->setScale(ui_scale);
	map_button->setPosition(getBoundingBox().size.width - layout.column_width/2 + 4, 34 * ui_scale);
	addChild(map_button, kDepthButton);
	auto onMapClick = EventListenerTouchOneByOne::create();
	onMapClick->setSwallowTouches(true);
	// trigger when you push down
	onMapClick->onTouchBegan = [this](Touch* touch, Event* event) -> bool {
		auto bounds = event->getCurrentTarget()->getBoundingBox();
		if (canClickMap && bounds.containsPoint(touch->getLocation())) {
			// TODO : Are you sure you want to go back to the map?
			PLAY_SOUND(kSoundEffect_UISelect);
			GameController::get()->setState(kStateMap);
			return true;
		}
		return false; // if you are consuming it
	};
	_eventDispatcher->addEventListenerWithSceneGraphPriority(onMapClick, map_button);
	
	mute_button = LoadSprite("icons/speaker.png");
	mute_button->setAnchorPoint(Vec2(0.5, 0.5));
	mute_button->setScale(ui_scale);
	mute_button->setPosition(layout.column_width/2 - 4, 34 * ui_scale);
	addChild(mute_button, kDepthButton);
	auto onMuteClick = EventListenerTouchOneByOne::create();
	onMuteClick->setSwallowTouches(true);
	// trigger when you push down
	onMuteClick->onTouchBegan = [this](Touch* touch, Event* event) -> bool {
		auto bounds = event->getCurrentTarget()->getBoundingBox();
		if (bounds.containsPoint(touch->getLocation())) {
			SoundManager::get()->toggleMute();
			PLAY_SOUND( kSoundEffect_UISelect );
			mute_button->setSpriteFrame(
				SoundManager::get()->getMute() ?
				"icons/speakercross.png" :
				"icons/speaker.png"
			);
			return true;
		}
		return false; // if you are consuming it
	};
	_eventDispatcher->addEventListenerWithSceneGraphPriority(onMuteClick, mute_button);
	
    this->scheduleUpdate();
    
    return true;
}

void ColumnScreen::setBackground(std::string texture) {
	background->setTexture(texture);
}
