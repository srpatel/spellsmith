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
#include "Level.hpp"

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

class OneShotDialog : public Dialog {
public:
	bool init(std::string message);
	
	CREATE_FUNC_1(OneShotDialog, std::string);
};

class PreLevelDialog : public Dialog {
public:
	bool init(RoundDef *);
	
	CREATE_FUNC_1(PreLevelDialog, RoundDef *);
};

class GotoMapConfirmationDialog : public Dialog {
public:
	bool init();
	
	CREATE_FUNC(GotoMapConfirmationDialog);
};
#endif /* Dialogs_hpp */
