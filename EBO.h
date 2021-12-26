#ifndef EBO_CLASS_H
#define EBO_CLASS_H

#include <glad/glad.h>

class EBO {
public:
	GLuint id;

	EBO(GLuint *indices, GLsizeiptr size);

	void bind();
	void unbind();
	void del();
};

#endif
