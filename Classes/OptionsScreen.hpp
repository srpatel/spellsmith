//
//  OptionsScreen.hpp
//  Gems
//
//  Created by Sunil Patel on 01/09/2017.
//
//

#ifndef OptionsScreen_hpp
#define OptionsScreen_hpp

#include "Screen.hpp"

class OptionsScreen : public Screen {
public:
	bool init();
	virtual void onSelect(){}
	CREATE_FUNC(OptionsScreen);
};
#endif /* OptionsScreen_hpp */
