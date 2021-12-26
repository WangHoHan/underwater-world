#ifndef VBO_CLASS_H
#define VBO_CLASS_H

#include <glad/glad.h>

class VBO {
	public:
		GLuint id;

		VBO(GLfloat *vertices, GLsizeiptr size);

		void bind();
		void unbind();
		void del();
};

#endif
