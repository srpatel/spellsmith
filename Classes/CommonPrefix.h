//
//  CommonPrefix.h
//  GemGame
//
//  Created by Sunil Patel on 06/11/2015.
//
//

#ifndef CommonPrefix_h
#define CommonPrefix_h

#ifdef __cplusplus
#include "cocos2d.h"
USING_NS_CC;
#endif

#if USE_SPRITE_SHEET
	#define TEXTURE_TYPE ui::Widget::TextureResType::PLIST
#else
	#define TEXTURE_TYPE ui::Widget::TextureResType::LOCAL
#endif
// || CC_PLATFORM_IOS || CC_PLATFORM_ANDROID

#define For(_i) for (int i = 0; i < _i; i++)
#define For2(_i, _j) for (int i = 0; i < _i; i++)\
for (int j = 0; j < _j; j++)

#define DEBUG 1
#if DEBUG
	#define LOG(_x_) do { printf(__FILE__ ":%d %s\n", __LINE__, _x_); } while (0)
	#define RAPIDJSON_ASSERT(_x_) do{assert(_x_);}while(0)
#else
	#define LOG(_x_)
	#define RAPIDJSON_ASSERT(_x_)
#endif

// Arg-based create funcs
#define CREATE_FUNC_1(__TYPE__, arg) \
static __TYPE__* create(arg __var) \
{ \
__TYPE__ *pRet = new(std::nothrow) __TYPE__(); \
if (pRet && pRet->init(__var)) \
{ \
pRet->autorelease(); \
return pRet; \
} \
else \
{ \
delete pRet; \
pRet = NULL; \
return NULL; \
} \
}

#define CREATE_FUNC_2(__TYPE__, arg1, arg2) \
static __TYPE__* create(arg1 __var1, arg2 __var2) \
{ \
__TYPE__ *pRet = new(std::nothrow) __TYPE__(); \
if (pRet && pRet->init(__var1, __var2)) \
{ \
pRet->autorelease(); \
return pRet; \
} \
else \
{ \
delete pRet; \
pRet = NULL; \
return NULL; \
} \
}

#define CREATE_FUNC_3(__TYPE__, arg1, arg2, arg3) \
static __TYPE__* create(arg1 __var1, arg2 __var2, arg3 __var3) \
{ \
__TYPE__ *pRet = new(std::nothrow) __TYPE__(); \
if (pRet && pRet->init(__var1, __var2, __var3)) \
{ \
pRet->autorelease(); \
return pRet; \
} \
else \
{ \
delete pRet; \
pRet = NULL; \
return NULL; \
} \
}

#ifdef __cplusplus
Sprite *LoadSprite(std::string);
#endif

#endif /* CommonPrefix_h */
