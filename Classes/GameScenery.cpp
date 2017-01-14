//
//  GameScenery.cpp
//  Gems
//
//  Created by Sunil Patel on 18/04/2016.
//
//

#include "GameScenery.hpp"
#include "Shaders.hpp"
#include "Constants.h"
#include "ImageManager.hpp"
#include "TextWisp.hpp"

bool GameScenery::init(Size size) {
	if ( !Layer::init() )
	{
		return false;
	}
	
	flags[0] = nullptr;
	flags[1] = nullptr;
	
	setContentSize(size);
	
	scenery = LoadSprite("ui/scenery.png");
	scenery->setAnchorPoint(Vec2(0.5, 0.5));
	float targetWidth = size.width;
	float actualWidth = scenery->getBoundingBox().size.width;
	float ratio = targetWidth/actualWidth;
	if (ratio > 1) {
		scenery->setScale(targetWidth/actualWidth);
	}
	scenery->setPosition(Vec2(size.width/2, (size.height)/2));
	addChild(scenery, -2);
	
	char_scale = size.height / scenery->getBoundingBox().size.height > 1 ?
						1 :
						size.height / scenery->getBoundingBox().size.height;
	
	redring = LoadSprite("ui/redring.png");
	redring->setAnchorPoint(Vec2(0.5, 0.2));
	addChild(redring);
	
	float ydiff = size.height;
	for (int i = 0; i < 3; i++) {
		enemy_positions3[2 - i].x = size.width - 25 - char_scale * 52.5 * i;
		enemy_positions3[2 - i].y = 8 + (i % 2) * ydiff * 0.1;
	}
	
	for (int i = 0; i < 2; i++) {
		enemy_positions2[1 - i].x = size.width - 25 - char_scale * 52.5 * (i + 0.5);
		enemy_positions2[1 - i].y = 8 + (i % 2) * ydiff * 0.1;
	}
	
	for (int i = 0; i < 1; i++) {
		enemy_positions1[i].x = size.width - 25 - char_scale * 52.5 * (i + 0.5);
		enemy_positions1[i].y = 8 + (i % 2) * ydiff * 0.1;
	}
	
	wizardsprite = spine::SkeletonAnimation::createWithFile("spine/wizard.json", ImageManager::get()->getAtlas(), 1.0f);
	wizardsprite->setPosition(50 * char_scale, 10 * char_scale);
	wizardsprite->setScale(char_scale * 0.4f);
	
	addChild(wizardsprite);
	
	Layer *message = Layer::create();
	message->setPosition(size/2);
	addChild(message, 300);
	banner = LoadSprite("ui/bannerlevelup.png");
	// Move banner up a bit - see mockup
	banner->setAnchorPoint(Vec2(0.5, 0.42));
	banner->setScale(char_scale);
	banner->setOpacity(0);
	message->addChild(banner);
	bigText = Label::createWithTTF( "", Fonts::TITLE_FONT, Fonts::TITLE_SIZE * char_scale);
	littleText = Label::createWithTTF( "", Fonts::TEXT_FONT, Fonts::TEXT_SIZE * char_scale);
	bigText->setPosition(0, 0);
	littleText->setPosition(0, 4-Fonts::TITLE_SIZE * char_scale);
	bigText->enableOutline(Color4B::BLACK, 1);
	littleText->enableOutline(Color4B::BLACK, 1);
	message->addChild(bigText);
	message->addChild(littleText);
	bigText->setOpacity(0);
	littleText->setOpacity(0);
	
	return true;
}

void GameScenery::showFlags(int flagType) {
	greyscaleMode(flagType != FLAG_TYPE_NONE);
	if (flagType == FLAG_TYPE_NONE) {
		// Animate flags away
		if (flags[0] != nullptr) {
			// animate them away!
			flags[0]->stopAllActions();
			flags[1]->stopAllActions();
			flags[0]->runAction(Sequence::create(
				MoveBy::create(0.1f, -Vec2(flags[0]->getContentSize().width, 0)),
				RemoveSelf::create(),
				nullptr));
			flags[1]->runAction(Sequence::create(
				MoveBy::create(0.1f, Vec2(flags[0]->getContentSize().width, 0)),
				RemoveSelf::create(),
				nullptr));
			flags[0] = nullptr;
			flags[1] = nullptr;
			
			auto fadeOut = EaseIn::create(FadeOut::create(0.3), 0.5);
			banner->runAction(fadeOut);
			bigText->runAction(fadeOut->clone());
			littleText->runAction(fadeOut->clone());
		}
	} else {
		auto fadeIn = EaseIn::create(FadeIn::create(0.3), 0.5);
		
		// Animate flags in
		if (flagType == FLAG_TYPE_WIN) {
			flags[0] = LoadSprite("ui/flag_levelup.png");
			flags[1] = LoadSprite("ui/flag_levelup.png");
			flags[0]->setScale(char_scale);
			flags[1]->setScale(-char_scale, char_scale);
			flags[0]->setAnchorPoint(Vec2(0, 0.5));
			flags[1]->setAnchorPoint(Vec2(0, 0.5));
			flags[0]->setPosition(-flags[0]->getContentSize().width, getContentSize().height/2.0);
			flags[1]->setPosition(getContentSize().width+flags[1]->getContentSize().width, getContentSize().height/2.0);
			
			// Fade banner in
			banner->runAction(fadeIn);
			
			
			bigText->setString("Level Up!");
			const char* sentence;
			int r = rand() % 5;
			switch (r) {
				case 0:	sentence = "Your knowledge expands..."; break;
				case 1:	sentence = "Another spell comes to mind"; break;
				case 2:	sentence = "Your power grows yet more"; break;
				case 3:	sentence = "Soon you shall be unstoppable!"; break;
				default:
				case 4:	sentence = "Just another day of goblin slaughter..."; break;
			}
			littleText->setString( sentence );
		} else if (flagType == FLAG_TYPE_LOSE) {
			flags[0] = LoadSprite("ui/flag_death_left.png");
			flags[1] = LoadSprite("ui/flag_death_right.png");
			flags[0]->setScale(char_scale);
			flags[1]->setScale(char_scale);
			flags[0]->setAnchorPoint(Vec2(0, 0.5));
			flags[1]->setAnchorPoint(Vec2(0, 0.5));
			flags[0]->setPosition(-flags[0]->getContentSize().width, getContentSize().height/2.0);
			flags[1]->setPosition(getContentSize().width, getContentSize().height/2.0);
			
			bigText->setString("Game Over!");
			const char* sentence;
			int r = rand() % 5;
			switch (r) {
				case 0:	sentence = "The goblins finally got their revenge!"; break;
				case 1:	sentence = "How...how is this possible!?"; break;
				case 2:	sentence = "Dead...for now..."; break;
				case 3:	sentence = "You will return, with a vengeance!"; break;
				default:
				case 4:	sentence = "Why does lightening bolt always roll 1?"; break;
			}
			littleText->setString( sentence );
		}
		
		bigText->runAction(fadeIn->clone());
		littleText->runAction(fadeIn->clone());
		
		flags[0]->runAction(EaseIn::create(MoveBy::create(0.3f, Vec2(flags[0]->getContentSize().width, 0)), 0.5));
		flags[1]->runAction(EaseIn::create(MoveBy::create(0.3f, -Vec2(flags[1]->getContentSize().width, 0)), 0.5));
		
		addChild(flags[0], 200);
		addChild(flags[1], 200);
	}
}

void GameScenery::greyscaleMode(bool grey) {
	redring->setVisible(! grey);
	GLProgram *shader = Shaders::greyscale();
	GLProgramState *state = GLProgramState::create(shader);
	state->setUniformFloat("u_start_time", _director->getTotalFrames() * _director->getAnimationInterval());
	state->setUniformInt("u_direction", grey ? 1 : -1);
	scenery->setGLProgramState(state);
	wizardsprite->setGLProgramState(state);
	if (enemies != nullptr)
		for (Enemy *e : *enemies) {
			e->sprite->setGLProgramState(state);
		}
}

void GameScenery::addTextWisp(Character *enemy, std::string s, Color3B c) {
	auto tw = TextWisp::create(s, c);
	Node *sprite = enemy->sprite;
	if (sprite == wizardsprite) {
		tw->setPosition(
			sprite->getPosition().x,
			sprite->getPosition().y + 295 * sprite->getScale());
	} else {
		tw->setPosition(sprite->getPosition().x, sprite->getPosition().y + sprite->getBoundingBox().size.height + 5);
	}
	addChild(tw);
}

void GameScenery::placeMonsters(std::vector<Enemy *> *e) {
	enemies = e;
	Vec2 *enemy_positions;
	if (enemies->size() == 3)
		enemy_positions = enemy_positions3;
	else if (enemies->size() == 2)
		enemy_positions = enemy_positions2;
	else
		enemy_positions = enemy_positions1;
	int i = 0;
	for (Enemy *enemy : *enemies) {
		enemy->sprite->setAnchorPoint(Vec2(0.77, 0));
		enemy->sprite->setPosition(enemy_positions[i++]);
		enemy->sprite->setScale(char_scale);
		enemy->buffHolder->setPosition(0.77 * enemy->sprite->getContentSize().width, enemy->sprite->getContentSize().height);
		addChild(enemy->sprite, 100);
	}
	if (enemies->size() == 3) {
		(*enemies)[1]->sprite->setLocalZOrder(50);
	} else if (enemies->size() == 2) {
		(*enemies)[0]->sprite->setLocalZOrder(50);
	}
	redring->setScale(char_scale);
	redring->setPosition((*enemies)[0]->sprite->getPosition());
}
void GameScenery::setSelected(int selected) {
	redring->setPosition((*enemies)[selected]->sprite->getPosition());
}
void GameScenery::setRedRingVisibility(bool visible) {
	redring->stopAllActions();
	auto time = 0.5f;
	redring->runAction(
		FadeTo::create(time, visible ? 255 : 0)
	);
}
GameScenery::~GameScenery() {
	redring->autorelease();
}
