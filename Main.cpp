#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Shader.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

GLfloat vertices[] = {
	-0.5f, -0.5f * float(sqrt(3)) * 1 / 3, 0.0f, 0.8f, 0.3f,  0.02f,
	 0.5f, -0.5f * float(sqrt(3)) * 1 / 3, 0.0f, 0.8f, 0.3f,  0.02f,
	 0.0f,  0.5f * float(sqrt(3)) * 2 / 3, 0.0f, 1.0f, 0.6f,  0.32f,
	-0.25f, 0.5f * float(sqrt(3)) * 1 / 6, 0.0f, 0.9f, 0.45f, 0.17f,
	 0.25f, 0.5f * float(sqrt(3)) * 1 / 6, 0.0f, 0.9f, 0.45f, 0.17f,
	 0.0f, -0.5f * float(sqrt(3)) * 1 / 3, 0.0f, 0.8f, 0.3f,  0.02f
};

GLuint indices[] = {
	0, 3, 5,
	3, 2, 4,
	5, 4, 1
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

	VAO1.linkAttrib(VBO1, 0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0);
	VAO1.linkAttrib(VBO1, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float)));

	VAO1.unbind();
	VBO1.unbind();
	EBO1.unbind();

	GLuint uniId = glGetUniformLocation(shaderProgram.id, "scale");

	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		shaderProgram.activate();
		glUniform1f(uniId, 0.5f);
		VAO1.bind();
		glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	VAO1.del();
	VBO1.del();
	EBO1.del();
	shaderProgram.del();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
