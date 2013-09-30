#include <GL/glew.h>

#ifndef SHADERFUNCTIONS_HPP_INCLUDED
#define SHADERFUNCTIONS_HPP_INCLUDED

void LinkProgram(GLuint id);

void CompileShader(GLuint id);

GLuint setupShaders();

#endif // SHADERFUNCTIONS_HPP_INCLUDED
