#include "VAO.h"


VAO::VAO() {
	glGenVertexArrays(1, &id);
}


void VAO::linkVBO(VBO VBO, GLuint layout) {
	VBO.bind();
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	VBO.unbind();
}


void VAO::bind() {
	glBindVertexArray(id);
}


void VAO::unbind() {
	glBindVertexArray(0);
}


void VAO::del() {
	glDeleteVertexArrays(1, &id);
}
