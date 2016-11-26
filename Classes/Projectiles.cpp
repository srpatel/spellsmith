//
//  BasicFire.cpp
//  Gems
//
//  Created by Sunil Patel on 06/08/2016.
//
//

#include "Projectiles.hpp"

// Px per second
#define PROJECTILE_SPEED 300

bool BasicProjectile::init(
		Vec2 from,
		Vec2 to,
		float scale,
		CallFunc* onHit
)
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
	Vector<SpriteFrame*> createAnimFrames(numShoot);
	char str[100] = {0};
	for(int i = 1; i <= numShoot; i++)
	{
		sprintf(str, "spells/%s/shoot/shoot%02d.png", path, i);
		auto frame = LoadSpriteFrame( std::string(str) );
		createAnimFrames.pushBack(frame);
	}
	
	Vector<SpriteFrame*> moveAnimFrames(numTravel);
	for(int i = 1; i <= numTravel; i++)
	{
		sprintf(str, "spells/%s/travel/travel%02d.png", path, i);
		auto frame = LoadSpriteFrame( std::string(str) );
		moveAnimFrames.pushBack(frame);
	}
	
	Vector<SpriteFrame*> hitAnimFrames(numHit);
	for(int i = 1; i <= numHit; i++)
	{
		sprintf(str, "spells/%s/hit/hit%02d.png", path, i);
		auto frame = LoadSpriteFrame( std::string(str) );
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
	
	
	return true;
}
