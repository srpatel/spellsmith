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
	bool init(Vec2 from, Vec2 to, CallFunc* onHit);
	CREATE_FUNC_3(BasicFire, Vec2, Vec2, CallFunc*);
};

#endif /* BasicFire_hpp */
