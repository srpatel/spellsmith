//
//  Buff.hpp
//  Gems
//
//  Created by Sunil Patel on 19/08/2017.
//
//

#ifndef Buff_hpp
#define Buff_hpp

class Character;

enum BuffType {
	BARRIER = 1,     // UNUSED!
	FREEZE,
	STUN,
	FURY,
	KINGS_COURT, // UNUSED!
	PHASING,
	FOCUS, // UNUSED!
	SPELL_FOCUS,
	CHARGE_BOLT
};

struct Buff {
	BuffType type;
	bool positive;
	std::string icon;
	Sprite *sprite;
	int turns; // -1 = forever, n = lasts n more turns
	int charges; // -1 = infinite, n = n charges remaining
	
	int priority;
	
	virtual void apply(Character *c) = 0;
	virtual void remove(Character *c) = 0;
	
	static Buff *createMudshield();
	static Buff *createFreeze(int amount);
	static Buff *createStun();
	static Buff *createFury();
	static Buff *createKingsCourt();
	static Buff *createPhasing();
	static Buff *createFocus();
	static Buff *createSpellFocus();
	static Buff *createChargeBolt();
	virtual ~Buff();
};

bool BuffComparator (Buff *left, Buff *right);

#endif /* Buff_hpp */
