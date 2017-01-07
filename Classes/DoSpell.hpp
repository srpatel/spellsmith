//
//  DoSpell.hpp
//  Gems
//
//  Created by Sunil Patel on 29/04/2016.
//
//

#ifndef DoSpell_hpp
#define DoSpell_hpp

#include "Spell.hpp"
#include "GameScene.hpp"

class DoSpell {
public:
	static void run(Game *, Spell *, Chain * , bool allowRepeats = true);
};

#endif /* DoSpell_hpp */
