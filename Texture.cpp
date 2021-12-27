#include "Texture.h"


Texture::Texture(const char* image, GLenum texType, GLuint slot, GLenum format, GLenum pixelType) {
	type = texType;
	int widthImg, heightImg, numColCh;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* bytes = stbi_load(image, &widthImg, &heightImg, &numColCh, 0);

	glGenTextures(1, &id);
	glActiveTexture(GL_TEXTURE0 + slot);
	unit = slot;
	glBindTexture(texType, id);

	glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(texType, 0, GL_RGBA, widthImg, heightImg, 0, format, pixelType, bytes);
	glGenerateMipmap(texType);

	stbi_image_free(bytes);
	glBindTexture(texType, 0);
}


void Texture::texUnit(Shader &shader, const char* uniform, GLuint unit) {
	GLuint tex0Uni = glGetUniformLocation(shader.id, uniform);
	shader.activate();
	glUniform1i(tex0Uni, 0);
}


void Texture::bind() {
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(type, id);
}


void Texture::unbind() {
	glBindTexture(type, 0);
}


void Texture::del() {
	glDeleteTextures(1, &id);
}
