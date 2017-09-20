//
//  Shaders.cpp
//  Gems
//
//  Created by Sunil Patel on 27/04/2016.
//
//

#include "Shaders.hpp"

#define SHADER(name, vert, frag)\
GLProgram *Shaders:: name () {\
	static GLProgram * p = nullptr;\
	if (p != nullptr) return p;\
	p = new GLProgram();\
	p->initWithFilenames("shaders/" #vert, "shaders/" #frag);\
	p->bindAttribLocation(GLProgram::ATTRIBUTE_NAME_POSITION, GLProgram::VERTEX_ATTRIB_POSITION);\
	p->bindAttribLocation(GLProgram::ATTRIBUTE_NAME_COLOR, GLProgram::VERTEX_ATTRIB_COLOR);\
	p->bindAttribLocation(GLProgram::ATTRIBUTE_NAME_TEX_COORD, GLProgram::VERTEX_ATTRIB_TEX_COORDS);\
	p->link();\
	p->updateUniforms();\
	return p;\
}

SHADER(none, none.vert, none.frag)
SHADER(smokey, none.vert, perlin.frag)
SHADER(greyscale, none.vert, grey.frag)
SHADER(smooth, none.vert, smooth.frag)
SHADER(bright, none.vert, bright.frag)
