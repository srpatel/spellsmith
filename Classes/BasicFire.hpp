//
//  BasicFire.hpp
//  Gems
//
//  Created by Sunil Patel on 06/08/2016.
//
//

#ifndef BasicFire_hpp
#define BasicFire_hpp

class BasicFire : public Layer {
public:
	bool init(Vec2 from, Vec2 to, float scale, CallFunc* onHit);
	CREATE_FUNC_4(BasicFire, Vec2, Vec2, float, CallFunc*);
};

#endif /* BasicFire_hpp */
