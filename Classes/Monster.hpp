//
//  Monster.hpp
//  Gems
//
//  Created by Sunil Patel on 26/11/2015.
//
//

#ifndef Monster_hpp
#define Monster_hpp

class Monster {};

class MonsterManager {
public:
	static MonsterManager *get();
	void init();
private:
	static MonsterManager *instance;
};

#endif /* Monster_hpp */
