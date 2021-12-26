#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cerrno>

std::string getFileContents(const char *filename);

class Shader {
	public:
		GLuint id;

		Shader(const char *vertexFile, const char *fragmentFile);
		
		void activate();
		void del();

	private:
		void compileErrors(unsigned int shader, const char* type);
};

#endif
