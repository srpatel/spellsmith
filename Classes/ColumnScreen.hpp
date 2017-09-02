//
//  ColumnScreen.hpp
//  Gems
//
//  Created by Sunil Patel on 31/08/2017.
//
//

#ifndef ColumnScreen_hpp
#define ColumnScreen_hpp

#include "GameScenery.hpp"
#include "Screen.hpp"

struct layout_t {
	float ui_scale;
	float column_height;
	float column_width;
	float bar_top_height;
	float bar_bottom_height;
	float scenery_height;
	Vec2 melee_spot; //where enemies stand to hit the wizard
};

class ColumnScreen : public Screen {
public:
	bool init(std::function<bool()> mapConfirm = {});
	virtual void onSelect();
	void setMapButtonVisible(bool);
	
	void setBackground(std::string);
protected:
	Sprite *background;
	layout_t layout;
	Sprite *bar_top, *right_col_sprite, *map_button, *mute_button;
private:
	bool canClickMap = true;
};

#endif /* ColumnScreen_hpp */
