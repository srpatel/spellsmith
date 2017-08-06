//
//  Map.hpp
//  Gems
//
//  Created by Sunil Patel on 24/06/2017.
//
//

#ifndef Map_hpp
#define Map_hpp

class MapScroll : public Layer {
public:
	bool init();
	CREATE_FUNC(MapScroll);
private:
	bool dragging;
	float downY;
	Layer *nodes;
	std::vector<EventListener *> listeners;
};

#endif /* Map_hpp */
