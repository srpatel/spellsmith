//
//  ClickMe.hpp
//  Gems
//
//  Created by Sunil Patel on 03/12/2017.
//

#ifndef ClickMe_hpp
#define ClickMe_hpp

class ClickMe : public Layer {
	bool init(bool big = false);
public:
	CREATE_FUNC(ClickMe);
	CREATE_FUNC_1(ClickMe, bool);
};

#endif /* ClickMe_hpp */
