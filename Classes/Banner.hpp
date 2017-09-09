//
//  Banner.hpp
//  Gems
//
//  Created by Sunil Patel on 09/09/2017.
//
//

#ifndef Banner_hpp
#define Banner_hpp

class Banner : public Layer {
public:
	bool init(std::string text, float width = -1);
	CREATE_FUNC_2(Banner, std::string, float);
	CREATE_FUNC_1(Banner, std::string);
};

#endif /* Banner_hpp */
