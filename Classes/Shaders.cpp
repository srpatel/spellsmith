//
//  Shaders.cpp
//  Gems
//
//  Created by Sunil Patel on 27/04/2016.
//
//

#include "Shaders.hpp"


GLProgram *Shaders::none() {
	static GLProgram * p = nullptr;
	if (p != nullptr) return p;
	p = new GLProgram();
	p->initWithFilenames("shaders/default.vert", "shaders/default.frag");
	p->bindAttribLocation(GLProgram::ATTRIBUTE_NAME_POSITION, GLProgram::VERTEX_ATTRIB_POSITION);
	p->bindAttribLocation(GLProgram::ATTRIBUTE_NAME_COLOR, GLProgram::VERTEX_ATTRIB_COLOR);
	p->bindAttribLocation(GLProgram::ATTRIBUTE_NAME_TEX_COORD, GLProgram::VERTEX_ATTRIB_TEX_COORDS);
	p->link();
	p->updateUniforms();
	return p;
}

GLProgram *Shaders::smokey() {
	static GLProgram * p = nullptr;
	if (p != nullptr) return p;
	p = new GLProgram();
	p->initWithFilenames("shaders/default.vert", "shaders/perlin.frag");
	p->bindAttribLocation(GLProgram::ATTRIBUTE_NAME_POSITION, GLProgram::VERTEX_ATTRIB_POSITION);
	p->bindAttribLocation(GLProgram::ATTRIBUTE_NAME_COLOR, GLProgram::VERTEX_ATTRIB_COLOR);
	p->bindAttribLocation(GLProgram::ATTRIBUTE_NAME_TEX_COORD, GLProgram::VERTEX_ATTRIB_TEX_COORDS);
	p->link();
	p->updateUniforms();
	return p;
}

GLProgram *Shaders::greyscale() {
	static GLProgram * p = nullptr;
	if (p != nullptr) return p;
	p = new GLProgram();
	p->initWithFilenames("shaders/default.vert", "shaders/grey.frag");
	p->bindAttribLocation(GLProgram::ATTRIBUTE_NAME_POSITION, GLProgram::VERTEX_ATTRIB_POSITION);
	p->bindAttribLocation(GLProgram::ATTRIBUTE_NAME_COLOR, GLProgram::VERTEX_ATTRIB_COLOR);
	p->bindAttribLocation(GLProgram::ATTRIBUTE_NAME_TEX_COORD, GLProgram::VERTEX_ATTRIB_TEX_COORDS);
	p->link();
	p->updateUniforms();
	return p;
}