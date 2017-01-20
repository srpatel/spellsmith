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

class RoundDef;

class SpellPicker : public Layer {
public:
	bool init(Spell *s1, Spell *);
	CREATE_FUNC_2(SpellPicker, Spell *, Spell *);
private:
};

// Ack want it here because it's similar to spell picker
class PostLevelDialog : public Layer {
public:
	bool init(RoundDef *);
	CREATE_FUNC_1(PostLevelDialog, RoundDef *);
private:
};

#endif /* SpellPicker_hpp */
