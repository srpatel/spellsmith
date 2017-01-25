//
//  Spellbook.cpp
//  Gems
//
//  Created by Sunil Patel on 18/01/2017.
//
//

#include "Spellbook.hpp"
#include "Spell.hpp"
#include "NavigationBar.hpp"
#include "GameScene.hpp"
#include "SaveData.hpp"
#include "Constants.h"

static layout_t layout;

bool Spellbook::init() {
	if ( !Layer::init() ) {
		return false;
	}
	
	Size visibleSize = Director::getInstance()->getVisibleSize();
	setContentSize(visibleSize);
	
	// COPIED FROM GAMESCENE
	auto right_col_sprite = LoadSprite("ui/column_right.png");
	
	
	layout.column_height = right_col_sprite->getBoundingBox().size.height;
	layout.column_width = right_col_sprite->getBoundingBox().size.width;
	
	GameScenery *scenery;
	
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
	// Scenery
	{
		layout.scenery_height = getBoundingBox().size.height - layout.column_height;
		
		scenery = GameScenery::create(Size(getBoundingBox().size.width, getBoundingBox().size.height - layout.column_height));
		scenery->hideObjects();
		scenery->setAnchorPoint(Vec2(0, 0));
		scenery->setPosition(0, layout.column_height);
		addChild(scenery);
	}
	// Gem background
	{
		auto grad = LayerColor::create();
		grad->initWithColor(Color4B(Colours::GEM_BACKGROUND));
		grad->setPosition(Director::getInstance()->getVisibleOrigin());
		grad->setContentSize(Size(getBoundingBox().size.width, layout.column_height));
		this->addChild(grad);
		
		spell_holder = Layer::create();
		spell_holder->setPosition(layout.column_width, NavigationBar::HEIGHT);
		addChild(spell_holder);
	}
	// Bar top
	{
		auto sprite = LoadSprite("ui/bar_top.png");
		layout.bar_top_height = sprite->getBoundingBox().size.height * ui_scale;
		sprite->setAnchorPoint(Vec2(0.5, 1));
		sprite->setPosition(Vec2(getBoundingBox().size.width/2, layout.column_height));
		sprite->setScale(ui_scale);
		this->addChild(sprite);
	}
	
	// bar bottom
	{
		auto sprite = LoadSprite("ui/bar_bottom.png");
		layout.bar_bottom_height = sprite->getBoundingBox().size.height * ui_scale;
		sprite->setAnchorPoint(Vec2(0.5, 0));
		sprite->setPosition(Vec2(getBoundingBox().size.width/2, 0));
		sprite->setScale(ui_scale);
		this->addChild(sprite);
	}
	
	// Columns
	{
		//auto sprite = LoadSprite("ui/column_right.png");
		right_col_sprite->setAnchorPoint(Vec2(1, 1));
		right_col_sprite->setPosition(Vec2(getBoundingBox().size.width, layout.column_height));
		right_col_sprite->setScale(ui_scale);
		this->addChild(right_col_sprite);
	}
	{
		auto sprite = LoadSprite("ui/column_left.png");
		sprite->setAnchorPoint(Vec2(0, 1));
		sprite->setPosition(Vec2(0, layout.column_height));
		sprite->setScale(ui_scale);
		this->addChild(sprite);
		
		auto grad = LayerColor::create();
		grad->initWithColor(Color4B(95, 91, 85, 255));
		grad->setPosition(Vec2(6, 340) * ui_scale);
		grad->setContentSize(Size(27, 30) * ui_scale);
		this->addChild(grad);
	}
	// END COPY
	
	return true;
}

void Spellbook::refreshSpells() {
	//
	spell_holder->removeAllChildren();
	int i = 0;
	int numberPerRow = 3;
	float startY = layout.column_height - layout.bar_top_height - NavigationBar::HEIGHT;
	float dy = 65;
	float dx = (getBoundingBox().size.width - 2 * layout.column_width) / numberPerRow;
	for (std::string spellname : SaveData::getSpells()) {
		printf("Adding spell %s\n", spellname.c_str());
		Spell *spell = SpellManager::get()->getByName(spellname);
		Layer *node = spell->makeNode(true);
		
		float currentX = dx * (i % numberPerRow) + dx/2.0;
		float currentY = startY - dy * (int) (i / numberPerRow) - dy/2.0;
		
		node->setPosition(currentX, currentY);
		spell_holder->addChild(node);
		
		auto label = Label::createWithTTF( spell->getName(), Fonts::TEXT_FONT, Fonts::SMALL_SIZE);
		label->setColor(Color3B::WHITE);
		label->setPosition(Vec2(currentX, currentY - 25.0));
		spell_holder->addChild(label, 1);
		
		// TODO:
		// - On tap, show spell info.
		// - Allow scrolling
		// - Allow drag to column
		// - Show box background around each spell
		
		i++;
	}
}
