//
//  Map.hpp
//  Gems
//
//  Created by Sunil Patel on 24/06/2017.
//
//

#ifndef Map_hpp
#define Map_hpp

class RoundDef;

class MapScroll : public Layer {
public:
	bool init();
	void refreshNodes();
	CREATE_FUNC(MapScroll);
private:
	bool dragging;
	float downY;
	std::map<RoundDef *, Sprite *> nodes;
};

#endif /* Map_hpp */
