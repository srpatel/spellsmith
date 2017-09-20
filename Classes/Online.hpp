//
//  Online.hpp
//  Gems
//
//  Created by Sunil Patel on 11/09/2017.
//
//

#ifndef Online_hpp
#define Online_hpp

struct ArenaScore {
	std::string name;
	std::string avatar;
	std::string killedby;
	int score;
	int rank; //ignored for submission
	std::vector<std::string> inventory;
};

class Online {
public:
	static Online *get();
	void submitArenaScore(ArenaScore);
	ArenaScore getMyArenaScore();
	std::vector<ArenaScore> getTopArenaScores();
private:
	Online();
	~Online();
};

#endif /* Online_hpp */
