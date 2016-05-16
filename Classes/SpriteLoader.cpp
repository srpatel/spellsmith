//
//  SpriteLoader.cpp
//  Gems
//
//  Created by Sunil Patel on 16/04/2016.
//
//

#if USE_SPRITE_SHEET
#define SPRITE(_path_) Sprite::createWithSpriteFrameName( _path_ )
#define SPRITE_FRAME(_path_, _r_) SpriteFrameCache::getInstance()->getSpriteFrameByName( _path_ )
#define SET_SPRITE_SCALE(_var_)
#else
#define SPRITE(_path_) Sprite::create( _path_ )
#define SPRITE_FRAME( _path_ , _r_) SpriteFrame::create( _path_ , _r_ )
#define SET_SPRITE_SCALE(_var_)
#endif

Sprite *LoadSprite(std::string path) {
	auto sprite = SPRITE(path);
	SET_SPRITE_SCALE(sprite);
	return sprite;
}

SpriteFrame *LoadSpriteFrame(std::string path, Rect r) {
	auto sprite = SPRITE_FRAME(path, r);
	return sprite;
}