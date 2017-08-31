//
//  Screen.hpp
//  Gems
//
//  Created by Sunil Patel on 31/08/2017.
//
//

#ifndef Screen_hpp
#define Screen_hpp

class Screen : public Layer {
public:
	virtual void onSelect() = 0;
};

#endif /* Screen_hpp */
