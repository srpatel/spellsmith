//
//  GameHUD.hpp
//  Gems
//
//  Created by Sunil Patel on 17/04/2016.
//
//

#ifndef GameHUD_hpp
#define GameHUD_hpp

#include "Characters.hpp"
#include "Monster.hpp"

class GameHUD : public Layer {
public:
	virtual bool init();
	virtual ~GameHUD();
	void setupMonsterList(std::vector<Enemy *> *);
	void setSelected(int);
	CREATE_FUNC(GameHUD);
private:
	std::vector<Enemy *> *enemies;
	Sprite *arrow;
};

class HealthBar : public Layer {
public:
	virtual bool init();
	void setPercentage(float);
	CREATE_FUNC(HealthBar);
private:
	Sprite *bg;
	Sprite *hp;
};

#endif /* GameHUD_hpp */
