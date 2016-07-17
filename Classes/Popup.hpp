//
//  Popup.hpp
//  Gems
//
//  Created by Sunil Patel on 17/07/2016.
//
//

#ifndef Popup_hpp
#define Popup_hpp

class Popup : public Layer {
public:
	bool init(float width, float height);
	CREATE_FUNC_2(Popup, float, float);
};

#endif /* Popup_hpp */
