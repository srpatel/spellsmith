//
//  SpellBlob.hpp
//  Gems
//
//  Created by Sunil Patel on 25/01/2017.
//
//

#ifndef SpellBlob_hpp
#define SpellBlob_hpp

#include "Spell.hpp"

class SpellBlob : public Layer {
public:
	bool init(Spell *, bool draggable);
	CREATE_FUNC_2(SpellBlob, Spell *, bool);
private:
	bool draggable;
	float distanceMoved;
};

#endif /* SpellBlob_hpp */
