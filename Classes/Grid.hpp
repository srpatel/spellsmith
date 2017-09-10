#ifndef Grid_hpp
#define Grid_hpp

#include "Gem.hpp"

struct Chain {
	int i;
	int j;
	GemType type;
	Chain *next;
};

class Grid : public cocos2d::Layer {
public:
	Grid(int w, int h, float maxWidth, float maxHeight);
	Gem *get(int column, int row);
	std::list<GemType> futureGems;
	bool init(float maxWidth, float maxHeight);
	void scramble(Chain *chain = nullptr);
	void preset(int which);
	void flashPreset(int which);
	void createRandomCrystalGems(int, Chain *);
	void makeCrystalsOverGemsOfType(GemType type, Chain *chain);
	void convertGemsOfType(GemType from, GemType to, Chain *chain);
	int destroyGemsOfType(GemType type, Chain *chain);
	void setActive(bool);
	bool isActive();
	cocos2d::Vec2 getSize();
	virtual bool onTouchBegan(cocos2d::Touch *, cocos2d::Event *);
	virtual void onTouchMoved(cocos2d::Touch *, cocos2d::Event *);
	virtual void onTouchEnded(cocos2d::Touch *, cocos2d::Event *);
protected:
	int width, height;
	Gem** grid;
	~Grid();
	
private:
	bool active;
	cocos2d::Touch* currentTouch = nullptr;
	Chain *chain = nullptr;
	LayerColor *overlay;
	Layer *selectedLayer;
	void set(int column, int row, Gem *gem, bool init, GemType);
	void refill();
	cocos2d::DrawNode *line;
	void cancelCurrentSpell();
	void castCurrentSpell();
	void drawChain();
	void drawSelected();
	void onTouchMovePart(Vec2 loc);
	bool doesLineIntersectCell(Vec2 start, Vec2 end, int column, int row);
	bool isAboveLine(Vec2 start, Vec2 end, Vec2 point);
	void initNewGem(Gem *gem, GemType type);
	
	bool diagonals_allowed;
};

#endif /* Grid_hpp */
