//
//  Spellbook.hpp
//  Gems
//
//  Created by Sunil Patel on 18/01/2017.
//
//

#ifndef Spellbook_hpp
#define Spellbook_hpp

class Spellbook : public Layer {
public:
	bool init();
	void refreshSpells();
	CREATE_FUNC(Spellbook);
private:
	Layer *spell_holder;
};


#endif /* Spellbook_hpp */
