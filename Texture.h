#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H

#include <glad/glad.h>
#include <stb/stb_image.h>
#include "Shader.h"

class Texture {
	public:
		GLuint id;
		const char* type;
		GLuint unit;

		Texture(const char* image, const char* texType, GLuint slot);


		void texUnit(Shader &shader, const char* uniform, GLuint unit);
		void bind();
		void unbind();
		void del();
};

#endif
