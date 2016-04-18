//
//  GameScenery.hpp
//  Gems
//
//  Created by Sunil Patel on 18/04/2016.
//
//

#ifndef GameScenery_hpp
#define GameScenery_hpp

class GameScenery : public Layer {
public:
	virtual bool init();
	virtual ~GameScenery();
	CREATE_FUNC(GameScenery);
private:
};

#endif /* GameScenery_hpp */
