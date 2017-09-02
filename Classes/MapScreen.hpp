//
//  MapScreen.hpp
//  Gems
//
//  Created by Sunil Patel on 16/01/2017.
//
//

#ifndef MapScreen_hpp
#define MapScreen_hpp

#include "Screen.hpp"

class MapScroll;

class MapScreen : public Screen {
public:
	bool init();
	virtual void onSelect();
	void refreshNodes();
	void scrollToUncomplete();
	CREATE_FUNC(MapScreen);
private:
	float mapMaxY, mapMinY;
	RoundDef *currentRound = nullptr;
	MapScroll *map;
};

#endif /* MapScreen_hpp */
