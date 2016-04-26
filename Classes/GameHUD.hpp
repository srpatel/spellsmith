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

class HealthBar;

class GameHUD : public Layer {
public:
	virtual bool init();
	virtual ~GameHUD();
	void setupMonsterList(std::vector<Enemy *> *);
	void setSelected(int);
	void updateHealthBars();
	void updateAttackClocks();
	CREATE_FUNC(GameHUD);
private:
	std::vector<Enemy *> *enemies;
	HealthBar *healthbars[3];
	Label *attackclocks[3];
	Sprite *attackclock_reds[3];
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
