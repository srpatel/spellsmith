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
	void setupMonsterList(std::vector<Enemy *>&);
	CREATE_FUNC(GameHUD);
private:
	Sprite *arrow;
};

#endif /* GameHUD_hpp */
