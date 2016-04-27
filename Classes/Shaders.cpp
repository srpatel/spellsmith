//
//  Shaders.cpp
//  Gems
//
//  Created by Sunil Patel on 27/04/2016.
//
//

#include "Shaders.hpp"

GLProgram *Shaders::smokey() {
	GLProgram * p = new GLProgram();
	p->initWithFilenames("shaders/default.vert", "shaders/perlin.frag");
	p->bindAttribLocation(GLProgram::ATTRIBUTE_NAME_POSITION, GLProgram::VERTEX_ATTRIB_POSITION);
	p->bindAttribLocation(GLProgram::ATTRIBUTE_NAME_COLOR, GLProgram::VERTEX_ATTRIB_COLOR);
	p->bindAttribLocation(GLProgram::ATTRIBUTE_NAME_TEX_COORD, GLProgram::VERTEX_ATTRIB_TEX_COORDS);
	p->link();
	p->updateUniforms();
	return p;
}