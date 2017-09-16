//
//  BasicFire.cpp
//  Gems
//
//  Created by Sunil Patel on 06/08/2016.
//
//

#include "Projectiles.hpp"

// Px per second
// #define PROJECTILE_SPEED 10
#define PROJECTILE_SPEED 300

BasicProjectile *BasicProjectile::makeFromType(ProjectileType type, Vec2 from, Vec2 to, float scale, CallFunc* onHit) {
	BasicProjectile *projectile;
	if (type == ptBasicWind) {
		projectile = BasicWind::create(from, to, scale, onHit);
	} else if (type == ptBasicEarth) {
		projectile = BasicEarth::create(from, to, scale, onHit);
	} else if (type == ptBasicWater) {
		projectile = BasicWater::create(from, to, scale, onHit);
	} else if (type == ptBasicFire) {
		projectile = BasicFire::create(from, to, scale, onHit);
	} else if (type == ptBasicPurple) {
		projectile = BasicPurple::create(from, to, scale, onHit);
	} else if (type == ptBasicMeteor) {
		projectile = BasicMeteor::create(from, to, 1, onHit);
	} else if (type == ptBasicDart) {
		projectile = BasicDart::create(from, to, 1, onHit);
	} else if (type == ptBasicAnvil) {
		projectile = BasicAnvil::create(from, to, 1, onHit);
	} else if (type == ptBasicIce) {
		projectile = BasicIce::create(from, to, 1, onHit);
	} else {
		printf("Unrecognised projectile type!\n");
		projectile = BasicFire::create(from, to, scale, onHit);
	}
	
	return projectile;
}

bool BasicAnim::init(
		Vec2 loc,
		float scale,
		CallFunc* onHit,
		AnimType animType
)
{
	if ( !Layer::init() )
	{
		return false;
	}
	
	this->setCascadeOpacityEnabled(true);
	
	auto sprite = Sprite::create();
	sprite->setPosition(loc);
	sprite->setScale(scale);
	// sprite->setContentSize();
	// Animate it!
	Vector<SpriteFrame*> frames(num);
	char str[100] = {0};
	for(int i = 1; i <= num; i++)
	{
		sprintf(str, "spells/%s/%s%02d.png", path, prefix, i);
		auto frame = LoadSpriteFrame( std::string(str) );
		frames.pushBack(frame);
	}

	auto animation = Animation::createWithSpriteFrames(frames, 0.1f);
	
	if (animType == AnimType::kLooping) {
		// Unsure the difference between these:
		// animation->setLoops(-1);
		sprite->runAction(
			RepeatForever::create(Animate::create(animation))
		);
	} else if (animType == AnimType::kSimple) {
		sprite->runAction(Sequence::create(
			Animate::create(animation),
			onHit,
			RemoveSelf::create(),
			nullptr
		));
	} else if (animType == AnimType::kPersistant) {
		sprite->runAction(Sequence::create(
			Animate::create(animation),
			onHit,
			nullptr
		));
	}
	addChild(sprite);
	
	
	return true;
}

bool BasicProjectile::init(
		Vec2 from,
		Vec2 to,
		float scale,
		CallFunc* onHit,
		float time
)
{
	if ( !Layer::init() )
	{
		return false;
	}
	auto scaleX = (from.x > to.x) ? -1 : 1;
	
	this->sprite = Sprite::create();
	
	sprite->setPosition(from + offset * scale * scaleX);
	sprite->setScaleY(scale);
	sprite->setScaleX(scale * scaleX);
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
	if (time < 0) time = from.distance(to) / PROJECTILE_SPEED;
	sprite->runAction(Sequence::create(
		Animate::create(createAnimation),
		Spawn::create(
			CallFunc::create([this, to, hitAnimFrames, onHit, time]() {
				// Move to target
				ActionInterval *moveTo = MoveTo::create(time, to);
				if (type == ptBasicAnvil) {
					moveTo = EaseIn::create(moveTo, 2);
				}
				sprite->runAction(Sequence::create(
					moveTo,
					onHit,
					CallFunc::create([this, hitAnimFrames, onHit]() {
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
						// Play sound effect
						if (sound) {
							PLAY_SOUND(sound);
						}
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
void BasicProjectile::setRotation(float r) {
	this->sprite->setRotation(r);
}
