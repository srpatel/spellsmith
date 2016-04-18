//
//  GameScenery.cpp
//  Gems
//
//  Created by Sunil Patel on 18/04/2016.
//
//

#include "GameScenery.hpp"

bool GameScenery::init() {
	if ( !Layer::init() )
	{
		return false;
	}
	
	return true;
}

GameScenery::~GameScenery() {
}