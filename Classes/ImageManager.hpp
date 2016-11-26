#ifndef ImageManager_hpp
#define ImageManager_hpp

class spAtlas;

class ImageManager {
public:
	static ImageManager *get();
	void init(spAtlas* atlas);
	inline spAtlas* getAtlas() { return atlas; }
private:
	static ImageManager *instance;
	spAtlas* atlas;
};

#endif /* ImageManager_hpp */
