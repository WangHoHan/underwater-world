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
	-0.5f, 0.0f,  0.5f, 0.83f, 0.70f, 0.44f, 0.0f, 0.0f,
	-0.5f, 0.0f, -0.5f, 0.83f, 0.70f, 0.44f ,5.0f, 0.0f,
	 0.5f, 0.0f, -0.5f, 0.83f, 0.70f, 0.44f, 0.0f, 0.0f,
	 0.5f, 0.0f,  0.5f, 0.83f, 0.70f, 0.44f, 5.0f, 0.0f,
	 0.0f, 0.8f,  0.0f, 0.92f, 0.86f, 0.76f, 2.5f, 5.0f
};

GLuint indices[] = {
	0, 1, 2,
	0, 2, 3,
	0, 1, 4,
	1, 2, 4,
	2, 3, 4,
	3, 0, 4
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

	VAO VAO1;
	VAO1.bind();

	VBO VBO1(vertices, sizeof(vertices));
	EBO EBO1(indices, sizeof(indices));

	VAO1.linkAttrib(VBO1, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
	VAO1.linkAttrib(VBO1, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	VAO1.linkAttrib(VBO1, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));

	VAO1.unbind();
	VBO1.unbind();
	EBO1.unbind();

	Texture brick("brick.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	brick.texUnit(shaderProgram, "tex0", 0);

	glEnable(GL_DEPTH_TEST);

	Camera camera(WINDOW_WIDTH, WINDOW_HEIGHT, glm::vec3(0.0f, 0.0f, 2.0f));

	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shaderProgram.activate();

		camera.inputs(window);
		camera.matrix(45.0f, 0.1f, 100.0f, shaderProgram, "camMatrix");

		brick.bind();
		VAO1.bind();
		glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, 0);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	VAO1.del();
	VBO1.del();
	EBO1.del();
	brick.del();
	shaderProgram.del();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
