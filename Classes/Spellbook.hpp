//
//  Spellbook.hpp
//  Gems
//
//  Created by Sunil Patel on 18/01/2017.
//
//

#ifndef Spellbook_hpp
#define Spellbook_hpp

#include "SpellBlob.hpp"

class Spellbook : public Layer {
public:
	bool init();
	void refreshSpells();
	void refreshEquips();
	CREATE_FUNC(Spellbook);
private:
	Layer *spell_holder, *equipped_holder;
	std::vector<EventListener *> equipped_listeners;
	std::vector<SpellBlob *> blobs;
};


#endif /* Spellbook_hpp */
