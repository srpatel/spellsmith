//
//  SpellPicker.hpp
//  Gems
//
//  Created by Sunil Patel on 10/07/2016.
//
//

#ifndef SpellPicker_hpp
#define SpellPicker_hpp

#include "Spell.hpp"

class SpellPicker : public Layer {
public:
	bool init(Spell *, Spell *);
	CREATE_FUNC_2(SpellPicker, Spell *, Spell *);
private:
};

#endif /* SpellPicker_hpp */
