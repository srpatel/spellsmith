//
//  Monster.hpp
//  Gems
//
//  Created by Sunil Patel on 26/11/2015.
//
//

#ifndef Monster_hpp
#define Monster_hpp

#include "spine/SkeletonAnimation.h"

enum AttackType {
	kAttackTypeMelee = 0,
	
	kAttackTypeHeal,
	kAttackTypeHealSelf,
	kAttackTypeHealOther,
	
	kAttackTypeProjectileFire,
	kAttackTypeProjectileWater,
	
	kAttackTypeOther,
};

class SkeletonDef {
public:
	std::string path;
	std::map<std::string, std::string> attachments;
	std::vector<std::string> skins;
};

class Attack {
public:
	Attack(int ratio, AttackType type, std::string animation, int amount);
	int ratio;
	AttackType type;
	std::string animation;
	int amount;
};
// this is just the monster as described by data.
// Enemy is the actual thing you fight against.
class Monster {
friend class MonsterManager;
public:
	std::string name;
	std::string getName();
	std::map<std::string, Vec2> offsets;
	int hp;
	std::string getSkeletonName();
	int attack_frequency;
	Attack *getAttack();
	Attack *getAttackFallback(Attack *a);
	int total_attack_ratio;
	std::vector<Attack *> attacks;
	spine::SkeletonAnimation *makeSkeleton();
private:
	SkeletonDef skeleton;
};

class MonsterManager {
public:
	static MonsterManager *get();
	Monster *get(std::string name);
	void init();
	std::unordered_map<std::string, Monster *> monsters;
private:
	static MonsterManager *instance;
};

#endif /* Monster_hpp */
