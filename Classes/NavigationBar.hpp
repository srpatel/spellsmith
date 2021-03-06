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
	void resetButtons();
	std::string focus;
	CREATE_FUNC(NavigationBar);
	//std::map<std::string, Vec2> buttonPositions;
private:
	bool enabled = true;
	Layer *buttonHolder;
};

#endif /* NavigationBar_hpp */
