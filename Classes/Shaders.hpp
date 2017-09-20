//
//  Shaders.hpp
//  Gems
//
//  Created by Sunil Patel on 27/04/2016.
//
//

#ifndef Shaders_hpp
#define Shaders_hpp

class Shaders {
public:
	static GLProgram *none();
	static GLProgram *smooth();
	static GLProgram *smokey();
	static GLProgram *greyscale();
	static GLProgram *bright();
};

#endif /* Shaders_hpp */
