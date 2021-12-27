#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Texture.h"
#include "Camera.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

GLfloat vertices[] = {
	-1.0f, 0.0f,  1.0f,		0.0f, 0.0f, 0.0f,		0.0f, 0.0f,		0.0f, 1.0f, 0.0f,
	-1.0f, 0.0f, -1.0f,		0.0f, 0.0f, 0.0f,		0.0f, 1.0f,		0.0f, 1.0f, 0.0f,
	 1.0f, 0.0f, -1.0f,		0.0f, 0.0f, 0.0f,		1.0f, 1.0f,		0.0f, 1.0f, 0.0f,
	 1.0f, 0.0f,  1.0f,		0.0f, 0.0f, 0.0f,		1.0f, 0.0f,		0.0f, 1.0f, 0.0f
};

GLuint indices[] = {
	0, 1, 2,
	0, 2, 3
};

GLfloat lightVertices[] = {
	-0.1f, 0.9f,  0.1f,
	-0.1f, 0.9f, -0.1f,
	 0.1f, 0.9f, -0.1f,
	 0.1f, 0.9f,  0.1f,
	-0.1f, 1.1f,  0.1f,
	-0.1f, 1.1f, -0.1f,
	 0.1f, 1.1f, -0.1f,
	 0.1f, 1.1f,  0.1f
};

GLuint lightIndices[] = {
	0, 1, 2,
	0, 2, 3,
	0, 4, 7,
	0, 7, 3,
	3, 7, 6,
	3, 6, 2,
	2, 6, 5,
	2, 5, 1,
	1, 5, 4,
	1, 4, 0,
	4, 5, 6,
	4, 6, 7
};


int main() {
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Underwater World", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	gladLoadGL();

	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	Shader shaderProgram("default.vert", "default.frag");

	VAO _VAO;
	_VAO.bind();

	VBO _VBO(vertices, sizeof(vertices));
	EBO _EBO(indices, sizeof(indices));

	_VAO.linkAttrib(_VBO, 0, 3, GL_FLOAT, 11 * sizeof(float), (void*)0);
	_VAO.linkAttrib(_VBO, 1, 3, GL_FLOAT, 11 * sizeof(float), (void*)(3 * sizeof(float)));
	_VAO.linkAttrib(_VBO, 2, 2, GL_FLOAT, 11 * sizeof(float), (void*)(6 * sizeof(float)));
	_VAO.linkAttrib(_VBO, 3, 3, GL_FLOAT, 11 * sizeof(float), (void*)(8 * sizeof(float)));

	_VAO.unbind();
	_VBO.unbind();
	_EBO.unbind();

	Shader lightShader("light.vert", "light.frag");

	VAO lightVAO;
	lightVAO.bind();

	VBO lightVBO(lightVertices, sizeof(lightVertices));
	EBO lightEBO(lightIndices, sizeof(lightIndices));

	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	lightVAO.linkAttrib(lightVBO, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);

	lightVAO.unbind();
	lightVBO.unbind();
	lightEBO.unbind();

	glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);

	glm::vec3 pyramidPos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::mat4 pyramidModel = glm::mat4(1.0f);
	lightModel = glm::translate(pyramidModel, pyramidPos);

	lightShader.activate();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.id, "model"), 1, GL_FALSE, glm::value_ptr(lightModel));
	glUniform4f(glGetUniformLocation(lightShader.id, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	shaderProgram.activate();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram.id, "model"), 1, GL_FALSE, glm::value_ptr(pyramidModel));
	glUniform4f(glGetUniformLocation(shaderProgram.id, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.id, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	Texture planksTex("planks.png", GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE);
	planksTex.texUnit(shaderProgram, "tex0", 0);

	Texture planksSpec("planksSpec.png", GL_TEXTURE_2D, 1, GL_RED, GL_UNSIGNED_BYTE);
	planksSpec.texUnit(shaderProgram, "tex1", 1);

	glEnable(GL_DEPTH_TEST);

	Camera camera(WINDOW_WIDTH, WINDOW_HEIGHT, glm::vec3(0.0f, 0.0f, 2.0f));

	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		camera.inputs(window);
		camera.updateMatrix(45.0f, 0.1f, 100.0f);

		shaderProgram.activate();
		glUniform3f(glGetUniformLocation(shaderProgram.id, "camPos"), camera.position.x, camera.position.y, camera.position.z);
		camera.matrix(shaderProgram, "camMatrix");

		planksTex.bind();
		planksSpec.bind();
		_VAO.bind();
		glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, 0);

		lightShader.activate();
		camera.matrix(lightShader, "camMatrix");
		lightVAO.bind();
		glDrawElements(GL_TRIANGLES, sizeof(lightIndices) / sizeof(int), GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	_VAO.del();
	_VBO.del();
	_EBO.del();
	planksTex.del();
	planksSpec.del();
	shaderProgram.del();
	lightVAO.del();
	lightVBO.del();
	lightEBO.del();
	lightShader.del();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
