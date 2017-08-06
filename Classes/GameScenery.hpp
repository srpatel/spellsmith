//
//  GameScenery.hpp
//  Gems
//
//  Created by Sunil Patel on 18/04/2016.
//
//

#ifndef GameScenery_hpp

#include "Characters.hpp"
#include "spine.h"

class GameScenery : public Layer {
public:
	static const int FLAG_TYPE_NONE = 0;
	static const int FLAG_TYPE_WIN  = 1;
	static const int FLAG_TYPE_LOSE = 2;
	virtual bool init(Size size);
	virtual ~GameScenery();
	void hideObjects();
	void placeMonsters(std::vector<Enemy *> *);
	void setSelected(int);
	void setRedRingVisibility(bool visible);
	void addTextWisp(Character *enemy, std::string s, Color3B c); // nullptr = wizard
	CREATE_FUNC_1(GameScenery, Size);
	spine::SkeletonAnimation *wizardsprite;
	void showFlags(int flagType);
	void setImage(std::string bg);
	float char_scale;
private:
	void greyscaleMode(bool);
	std::vector<Enemy *> *enemies;
	Sprite *redring;
	Sprite *scenery;
	Vec2 enemy_positions3[3];
	Vec2 enemy_positions2[3];
	Vec2 enemy_positions1[3];
	Sprite *flags[2];
	Label *littleText;
	Label *bigText;
	Sprite *banner;
};

#endif /* GameScenery_hpp */
