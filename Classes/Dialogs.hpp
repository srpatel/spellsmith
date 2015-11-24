//
//  Dialogs.hpp
//  Gems
//
//  Created by Sunil Patel on 11/11/2015.
//
//

#ifndef Dialogs_hpp
#define Dialogs_hpp

#define SIMPLE_DIALOG(_x_) class _x_ : public Dialog {\
public:\
bool init();\
\
CREATE_FUNC(_x_);\
}

class Dialog : public Layer {
public:
	bool init();
	
	virtual bool onTouchBegan(cocos2d::Touch *, cocos2d::Event *);
	virtual void onTouchMoved(cocos2d::Touch *, cocos2d::Event *);
	virtual void onTouchEnded(cocos2d::Touch *, cocos2d::Event *);
};

SIMPLE_DIALOG(LevelWonDialog);
SIMPLE_DIALOG(LevelLostDialog);
SIMPLE_DIALOG(SpellInfoDialog);

#endif /* Dialogs_hpp */
