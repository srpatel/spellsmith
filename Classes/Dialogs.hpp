//
//  Dialogs.hpp
//  Gems
//
//  Created by Sunil Patel on 11/11/2015.
//
//

#ifndef Dialogs_hpp
#define Dialogs_hpp

class Dialog : public Layer {};

class SpellInfoDialog : public Dialog {
public:
	bool init();
	
	virtual bool onTouchBegan(cocos2d::Touch *, cocos2d::Event *);
	virtual void onTouchMoved(cocos2d::Touch *, cocos2d::Event *);
	virtual void onTouchEnded(cocos2d::Touch *, cocos2d::Event *);
	
	CREATE_FUNC(SpellInfoDialog);
};

#endif /* Dialogs_hpp */
