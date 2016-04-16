//
//  SpriteLoader.cpp
//  Gems
//
//  Created by Sunil Patel on 16/04/2016.
//
//

#if USE_SPRITE_SHEET
#define SPRITE(_path_) Sprite::createWithSpriteFrameName( _path_ )
#define SET_SPRITE_SCALE(_var_)
#else
#define SPRITE(_path_) Sprite::create( _path_ )
#define SET_SPRITE_SCALE(_var_)
#endif

Sprite *LoadSprite(const char *path) {
	auto sprite = SPRITE(path);
	SET_SPRITE_SCALE(sprite);
	return sprite;
}