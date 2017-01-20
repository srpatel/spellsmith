//
//  MapScreen.hpp
//  Gems
//
//  Created by Sunil Patel on 16/01/2017.
//
//

#ifndef MapScreen_hpp
#define MapScreen_hpp

class MapScreen : public Layer {
public:
	bool init();
	void refreshNodes();
	CREATE_FUNC(MapScreen);
private:
	Layer *nodes;
	std::vector<EventListener *> listeners;
};

#endif /* MapScreen_hpp */
