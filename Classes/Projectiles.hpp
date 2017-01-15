//
//  BasicFire.hpp
//  Gems
//
//  Created by Sunil Patel on 06/08/2016.
//
//

#ifndef BasicFire_hpp
#define BasicFire_hpp

#define PROJECTILE(_classname, _name, _shoot, _travel, _hit, _offset) \
class _classname : public BasicProjectile { \
public: \
	_classname() \
		: BasicProjectile(#_name, _shoot, _travel, _hit, _offset) {} \
	inline bool init(Vec2 from, Vec2 to, float scale, CallFunc* onHit){\
		return BasicProjectile::init(from, to, scale, onHit);\
	}\
	CREATE_FUNC_4(_classname, Vec2, Vec2, float, CallFunc*);\
}

#define ANIM(_classname, _name, _num) \
class _classname : public BasicAnim { \
public: \
	_classname() \
		: BasicAnim(#_name, _num) {} \
	inline bool init(Vec2 loc, float scale, CallFunc* onHit){\
		return BasicAnim::init(loc, scale, onHit);\
	}\
	CREATE_FUNC_3(_classname, Vec2, float, CallFunc*);\
}

class BasicProjectile : public Layer {
public:
	BasicProjectile(const char *path, int numShoot, int numTravel, int numHit, Vec2 offset) :
		path(path), numShoot(numShoot), numTravel(numTravel), numHit(numHit), offset(offset){}
	bool init(
		Vec2 from,
		Vec2 to,
		float scale,
		CallFunc* onHit);
protected:
	Vec2 offset;
	const char *path;
	int numShoot;
	int numTravel;
	int numHit;
};

class BasicAnim : public Layer {
public:
	BasicAnim(const char *path, int num) :
		path(path), num(num){}
	bool init(
		Vec2 loc,
		float scale,
		CallFunc* onHit);
protected:
	const char *path;
	int num;
};

// TODO: Resize everything to be 150x150, positioning as needed.
PROJECTILE(BasicFire, fireball,   /*shoot*/ 19, /*travel*/ 4, /*hit*/ 10, /*offset*/ Vec2( 0, 5));
PROJECTILE(BasicWind, windball,   /*shoot*/ 13, /*travel*/ 4, /*hit*/  9, /*offset*/ Vec2(20, 0));
PROJECTILE(BasicEarth, earthball, /*shoot*/ 28, /*travel*/ 1, /*hit*/ 23, /*offset*/ Vec2( 0, 0));
PROJECTILE(BasicWater, waterball, /*shoot*/ 13, /*travel*/ 3, /*hit*/ 10, /*offset*/ Vec2(20, 0));

ANIM(AnimHeal, heal, 11);

#endif /* BasicFire_hpp */
