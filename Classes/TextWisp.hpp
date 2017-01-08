//
//  TextWisp.hpp
//  Gems
//
//  Created by Sunil Patel on 08/01/2017.
//
//

#ifndef TextWisp_hpp
#define TextWisp_hpp

class TextWisp : public Layer {
	bool init(std::string text, Color3B colour);
public:
	CREATE_FUNC_2(TextWisp, std::string, Color3B);
};

#endif /* TextWisp_hpp */
