#ifndef Grid_hpp
#define Grid_hpp

#include "Gem.hpp"
#include "EffectSprite/LightEffect.h"

struct Chain {
	int i;
	int j;
	GemType type;
	Chain *next;
};

class Grid : public cocos2d::Layer {
public:
    Grid(int w, int h, float width);
    Gem *get(int column, int row);
    bool init(float width);
	bool active;
	void scramble();
    cocos2d::Vec2 getSize();
    virtual bool onTouchBegan(cocos2d::Touch *, cocos2d::Event *);
	virtual void onTouchMoved(cocos2d::Touch *, cocos2d::Event *);
	virtual void onTouchEnded(cocos2d::Touch *, cocos2d::Event *);
protected:
    int width, height;
    Gem** grid;
    
    ~Grid();
	
private:
	cocos2d::Touch* currentTouch = nullptr;
	Chain *chain = nullptr;
    void set(int column, int row, Gem *gem, bool);
    void refill();
	cocos2d::DrawNode *line;
	void cancelCurrentSpell();
	void castCurrentSpell();
	void drawChain();
	void drawSelected();
	void onTouchMovePart(Vec2 loc);
	bool doesLineIntersectCell(Vec2 start, Vec2 end, int column, int row);
	bool isAboveLine(Vec2 start, Vec2 end, Vec2 point);
	
	bool diagonals_allowed;
};

#endif /* Grid_hpp */
