//
//  Constants.h
//  GemGame
//
//  Created by Sunil Patel on 08/11/2015.
//
//

#ifndef Constants_h
#define Constants_h

namespace Fonts {
//Adobe Garamond Pro,
	const std::string TITLE_FONT = "fonts/eb_garamond.ttf";//"fonts/im_fell_english_sc.ttf";
	const std::string TEXT_FONT = "fonts/eb_garamond.ttf";
	const std::string TEXT_FONT_ITALIC = "fonts/eb_garamond_italic.ttf";
	const std::string NUMBER_FONT = "fonts/baskerville.ttc";
	
	const int TITLE_SIZE = 32;
	const int TEXT_SIZE = 18;
	const int SMALL_SIZE = 12;
};

namespace Colours {
	const Color3B BACKGROUND     ( 36, 14, 13    );
	const Color3B UI_BACKGROUND  (115,109, 99    );
	const Color3B GEM_BACKGROUND ( 46, 19, 19    );
	const Color4B SEMIBLACK      (  0,  0,  0, 60);
	const Color3B ORANGE         (232,165, 62    );
};


#endif /* Constants_h */
