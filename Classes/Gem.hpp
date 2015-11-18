#ifndef Gem_hpp
#define Gem_hpp

enum GemType {
	NONE = 0,
	
	FIRE = 1,
	EARTH,
	WATER,
	AIR
};

struct Gem {
    GemType type;
    Sprite* sprite;
    int downBy = 0;
    bool didInit = false;
    void init();
	void init(GemType type);
	
	static float getWidth();
	static float getHeight();
	static float scale;
};

#endif /* Gem_hpp */
