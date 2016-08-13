//
//  BasicFire.cpp
//  Gems
//
//  Created by Sunil Patel on 06/08/2016.
//
//

#include "BasicFire.hpp"

// Px per second
#define PROJECTILE_SPEED 300

bool BasicFire::init(Vec2 from, Vec2 to, float scale, CallFunc *onHit)
{
	if ( !Layer::init() )
	{
		return false;
	}
	
	auto sprite = Sprite::create();
	sprite->setPosition(from);
	sprite->setScale(scale);
	// sprite->setContentSize();
	// Animate it!
	auto size = 19;
	Vector<SpriteFrame*> createAnimFrames(size);
	char str[100] = {0};
	for(int i = 1; i <= size; i++)
	{
		sprintf(str, "spells/fireball/shoot/shoot%02d.png", i);
		auto frame = LoadSpriteFrame( std::string(str), Rect(0, 0, 80 / CC_CONTENT_SCALE_FACTOR(), 100 / CC_CONTENT_SCALE_FACTOR()) );
		createAnimFrames.pushBack(frame);
	}
	
	size = 4;
	Vector<SpriteFrame*> moveAnimFrames(size);
	for(int i = 1; i <= size; i++)
	{
		sprintf(str, "spells/fireball/travel/travel%02d.png", i);
		auto frame = LoadSpriteFrame( std::string(str), Rect(0, 0, 80 / CC_CONTENT_SCALE_FACTOR(), 100 / CC_CONTENT_SCALE_FACTOR()) );
		moveAnimFrames.pushBack(frame);
	}
	
	size = 10;
	Vector<SpriteFrame*> hitAnimFrames(size);
	for(int i = 1; i <= size; i++)
	{
		sprintf(str, "spells/fireball/hit/hit%02d.png", i);
		auto frame = LoadSpriteFrame( std::string(str), Rect(0, 0, 80 / CC_CONTENT_SCALE_FACTOR(), 100 / CC_CONTENT_SCALE_FACTOR()) );
		hitAnimFrames.pushBack(frame);
	}

	auto createAnimation = Animation::createWithSpriteFrames(createAnimFrames, 0.02f);
	auto moveAnimation = Animation::createWithSpriteFrames(moveAnimFrames, 0.1f);
	moveAnimation->setLoops(-1);
	onHit->retain();
	float time = from.distance(to) / PROJECTILE_SPEED;
	sprite->runAction(Sequence::create(
		Animate::create(createAnimation),
		Spawn::create(
			CallFunc::create([this, to, sprite, hitAnimFrames, onHit, time]() {
				// Move to target
				sprite->runAction(Sequence::create(
					MoveTo::create(time, to),
					onHit,
					CallFunc::create([this, sprite, hitAnimFrames, onHit]() {
						// deal damage or whatever
						// animate sprite, and remove afterwards
						onHit->autorelease();
						auto hitAnimation = Animation::createWithSpriteFrames(hitAnimFrames, 0.02f);
						sprite->stopAllActions();
						sprite->runAction(Sequence::create(
							Animate::create(hitAnimation),
							CallFunc::create([this]() {
								removeFromParent();
							}),
							nullptr
						));
					}),
					nullptr
				));
			}),
			Animate::create(moveAnimation),
			nullptr
		),
		nullptr
	));
	addChild(sprite);
	
	// Make three sprites:
	// - create
	// - move
	// - hit
	// On create, need to know:
	// - start pos
	// - end pos
	// - run action on finish?
	
	
	return true;
}