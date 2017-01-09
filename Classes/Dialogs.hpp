//
//  Dialogs.hpp
//  Gems
//
//  Created by Sunil Patel on 11/11/2015.
//
//

#ifndef Dialogs_hpp
#define Dialogs_hpp

#include "Spell.hpp"

#define SIMPLE_DIALOG(_x_) class _x_ : public Dialog {\
public:\
bool init();\
\
CREATE_FUNC(_x_);\
}

class Dialog : public Layer {
public:
	bool init(bool closeable, bool captureTouch, float centralWidth, float centralHeight);
	
	virtual bool onTouchBegan(cocos2d::Touch *, cocos2d::Event *);
	virtual void onTouchMoved(cocos2d::Touch *, cocos2d::Event *);
	virtual void onTouchEnded(cocos2d::Touch *, cocos2d::Event *);
private:
	bool closeable, captureTouch;
	float centralWidth, centralHeight;
};

SIMPLE_DIALOG(OptionsDialog);

class SpellInfoDialog : public Dialog {
public:
	bool init(Spell *);
	
	CREATE_FUNC_1(SpellInfoDialog, Spell *);
};

#endif /* Dialogs_hpp */
