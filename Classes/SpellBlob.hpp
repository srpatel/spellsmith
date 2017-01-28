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
	bool init(Spell *, bool draggable, std::function<void(int, Spell*)> onSelect, std::function<bool(Spell*)> isBeingUsed);
	void refresh();
	CREATE_FUNC_4(SpellBlob, Spell *, bool, std::function<void(int, Spell*)>, std::function<bool(Spell*)>);
private:
	std::function<bool(Spell*)> isBeingUsed;
	Spell *spell;
	Node *mininode, *mininode_grey;
	bool draggable;
	bool dragging;
	float distanceMoved;
};

#endif /* SpellBlob_hpp */
