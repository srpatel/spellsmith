//
//  BasicFire.hpp
//  Gems
//
//  Created by Sunil Patel on 06/08/2016.
//
//

#ifndef BasicFire_hpp
#define BasicFire_hpp

#include "SoundManager.hpp"

#define PROJECTILE(_classname, _name, _shoot, _travel, _hit, _offset, _sound) \
class _classname : public BasicProjectile { \
public: \
	_classname() \
		: BasicProjectile(#_name, _shoot, _travel, _hit, _offset, _sound) {} \
	inline bool init(Vec2 from, Vec2 to, float scale, CallFunc* onHit){\
		return BasicProjectile::init(from, to, scale, onHit);\
	}\
	CREATE_FUNC_4(_classname, Vec2, Vec2, float, CallFunc*);\
}

#define ANIM(_classname, _name, _num, _fileprefix) \
class _classname : public BasicAnim { \
public: \
	_classname() \
		: BasicAnim(_name, _num, _fileprefix) {} \
	inline bool init(Vec2 loc, float scale, CallFunc* onHit){\
		return BasicAnim::init(loc, scale, onHit);\
	}\
	CREATE_FUNC_3(_classname, Vec2, float, CallFunc*);\
}

class BasicProjectile : public Layer {
public:
	BasicProjectile(const char *path, int numShoot, int numTravel, int numHit, Vec2 offset, const char *sound) :
		path(path), numShoot(numShoot), numTravel(numTravel), numHit(numHit), offset(offset), sound(sound){}
	bool init(
		Vec2 from,
		Vec2 to,
		float scale,
		CallFunc* onHit);
public:
	void setRotation(float r);
protected:
	Vec2 offset;
	const char *path;
	const char *sound;
	int numShoot;
	int numTravel;
	int numHit;
private:
	Sprite* sprite;
};

class BasicAnim : public Layer {
public:
	BasicAnim(const char *path, int num, const char *prefix) :
		path(path), num(num), prefix(prefix){}
	bool init(
		Vec2 loc,
		float scale,
		CallFunc* onHit);
protected:
	const char *path;
	const char *prefix;
	int num;
};

// TODO: Resize everything to be 150x150, positioning as needed.
PROJECTILE(BasicFire,   fireball,  /*shoot*/ 19, /*travel*/ 4, /*hit*/ 10, /*offset*/ Vec2( 0, 5), kSoundEffect_PHFire);
PROJECTILE(BasicWind,   windball,  /*shoot*/ 13, /*travel*/ 4, /*hit*/  9, /*offset*/ Vec2(20, 0), kSoundEffect_PHAir);
PROJECTILE(BasicEarth,  earthball, /*shoot*/ 28, /*travel*/ 1, /*hit*/ 23, /*offset*/ Vec2( 0, 0), kSoundEffect_PHEarth);
PROJECTILE(BasicWater,  waterball, /*shoot*/ 13, /*travel*/ 3, /*hit*/ 10, /*offset*/ Vec2(20, 0), kSoundEffect_PHWater);
PROJECTILE(BasicMeteor, meteor,    /*shoot*/  0, /*travel*/ 3, /*hit*/  9, /*offset*/ Vec2(20, 0), kSoundEffect_PHFire);
PROJECTILE(BasicPurple, drainlife,    /*shoot*/  11, /*travel*/ 3, /*hit*/  9, /*offset*/ Vec2(10, 0), kSoundEffect_PHWater);

ANIM(AnimHeal, "castheal", 11, "castheal");
ANIM(AnimFire, "castfire", 11, "castfire");
ANIM(AnimLightning1, "lightning/1", 11, "LightningStrike100");
ANIM(AnimLightning2, "lightning/2", 11, "LightningStrike200");
ANIM(AnimLightning3, "lightning/3", 11, "LightningStrike300");

#endif /* BasicFire_hpp */
