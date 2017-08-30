//
//  MapScreen.hpp
//  Gems
//
//  Created by Sunil Patel on 16/01/2017.
//
//

#ifndef MapScreen_hpp
#define MapScreen_hpp

class MapScroll;

class MapScreen : public Layer {
public:
	bool init();
	void refreshNodes();
	void scrollToUncomplete();
	CREATE_FUNC(MapScreen);
private:
	float mapMaxY, mapMinY;
	MapScroll *map;
};

#endif /* MapScreen_hpp */
