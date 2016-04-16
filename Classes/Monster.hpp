//
//  Monster.hpp
//  Gems
//
//  Created by Sunil Patel on 26/11/2015.
//
//

#ifndef Monster_hpp
#define Monster_hpp

enum AttackType {
	kAttackTypeMelee = 0
};

class Attack {
public:
	Attack(int frequency, AttackType type, int amount);
	int frequency;
	AttackType type;
	int amount;
};
// this is just the monster as described by data.
// Enemy is the actual thing you fight against.
class Monster {
public:
	std::string name;
	Sprite *sprite;
	int hp;
	std::vector<Attack *> attacks;
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
