//
//  GameScenery.hpp
//  Gems
//
//  Created by Sunil Patel on 18/04/2016.
//
//

#ifndef GameScenery_hpp

#include "Characters.hpp"

class GameScenery : public Layer {
public:
	virtual bool init(Size size);
	virtual ~GameScenery();
	void placeMonsters(std::vector<Enemy *> *);
	void setSelected(int);
	void greyscaleMode(bool);
	CREATE_FUNC_1(GameScenery, Size);
	Sprite *wizardsprite;
private:
	std::vector<Enemy *> *enemies;
	Sprite *redring;
	Sprite *scenery;
	Vec2 enemy_positions3[3];
	Vec2 enemy_positions2[3];
	Vec2 enemy_positions1[3];
	float char_scale;
};

#endif /* GameScenery_hpp */
