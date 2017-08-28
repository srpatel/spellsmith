//
//  NavigationBar.hpp
//  Gems
//
//  Created by Sunil Patel on 12/11/2015.
//
//

#ifndef NavigationBar_hpp
#define NavigationBar_hpp

class NavigationBar : public Layer {
public:
	static const int HEIGHT = 55;
	bool init();
	inline void setEnabled(bool enabled) {this->enabled = enabled;}
	
	CREATE_FUNC(NavigationBar);
private:
	bool enabled = true;
};

#endif /* NavigationBar_hpp */
