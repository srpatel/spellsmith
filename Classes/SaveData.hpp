//
//  SaveData.hpp
//  Gems
//
//  Created by Sunil Patel on 25/11/2015.
//
//

#ifndef SaveData_hpp
#define SaveData_hpp

#include "Level.hpp"

class SaveData {
	// Get level scores
	int getLevelScore(Level *);
	int getInfScore();
	
	void setLevelScore(Level *, int);
	void setInfScore(int);
};

#endif /* SaveData_hpp */
