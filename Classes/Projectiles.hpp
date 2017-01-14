//
//  BasicFire.hpp
//  Gems
//
//  Created by Sunil Patel on 06/08/2016.
//
//

#ifndef BasicFire_hpp
#define BasicFire_hpp

#define PROJECTILE(_classname, _name, _shoot, _travel, _hit) \
class _classname : public BasicProjectile { \
public: \
	_classname() \
		: BasicProjectile(#_name, _shoot, _travel, _hit) {} \
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
	BasicProjectile(const char *path, int numShoot, int numTravel, int numHit) :
		path(path), numShoot(numShoot), numTravel(numTravel), numHit(numHit){}
	bool init(
		Vec2 from,
		Vec2 to,
		float scale,
		CallFunc* onHit);
protected:
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
PROJECTILE(BasicFire, fireball, 19, 4, 10);
PROJECTILE(BasicWind, windball, 13, 4, 9);
PROJECTILE(BasicEarth, earthball, 28, 1, 23);
PROJECTILE(BasicWater, waterball, 13, 3, 10);

ANIM(AnimHeal, heal, 11);

#endif /* BasicFire_hpp */
