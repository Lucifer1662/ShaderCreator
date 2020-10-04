#pragma once
#include <GL\glew.h>
#include <string>
#include <memory>
#include <fstream>

using std::string;

GLuint CreateShader(GLenum type, string source);

unsigned int CreateProgram(const string& vsCode, const string& fsCode);
unsigned int CreateProgram(const string& vsCode, const string& gsCode, const string& fsCode);
unsigned int CreateProgram(const string & computeCode);

std::string readProgram(const string path);

struct Program {
	

	unsigned int program;

	Program(int typeId, bool load);
	~Program();
};


