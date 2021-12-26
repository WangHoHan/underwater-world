#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

int main() {
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// creating window
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Underwater World", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// introduce window into current context
	glfwMakeContextCurrent(window);
	
	gladLoadGL();

	// specify viewport of OpenGL
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	// specify window background color
	glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
	// clear back buffer and assign new color to it
	glClear(GL_COLOR_BUFFER_BIT);
	// swapping back buffer with front buffer
	glfwSwapBuffers(window);

	// program loop
	while (!glfwWindowShouldClose(window)) {
		// taking care of all events like dragging window
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}