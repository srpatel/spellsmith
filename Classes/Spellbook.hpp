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
#include "ColumnScreen.hpp"

class Spellbook : public ColumnScreen {
	friend class Tutorial;
public:
	bool init();
	virtual void onSelect();
	void refreshSpells();
	void refreshEquips();
	void gotoPageWithSpell(std::string name);
	CREATE_FUNC(Spellbook);
	Sprite *mute_button;
private:
	Layer *spell_holder, *equipped_holder;
	std::vector<EventListener *> equipped_listeners;
	std::vector<SpellBlob *> blobs;
	Node *left;
	Node *right;
	Label *page_label;
	int current_page;
	int num_pages;
	Vec2 startPos;
};


#endif /* Spellbook_hpp */
