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

#endif /* CommonPrefix_h */
