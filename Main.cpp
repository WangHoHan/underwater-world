#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

// vertex Shader source code
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
// fragment Shader source code
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(0.8f, 0.3f, 0.02f, 1.0f);\n"
"}\n\0";

int main() {
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// vertices coordinates
	GLfloat vertices[] = {
		-0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f,
		0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f,
		0.0f, 0.5f * float(sqrt(3)) * 2 / 3, 0.0f
	};

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

	//create vertex shader object and get its reference
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	// attach vertex shader source to vertex shader object
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	// compile vertex shader into machine code
	glCompileShader(vertexShader);

	//create fragment shader object and get its reference
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	// attach fragment shader source to vertex shader object
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	// compile fragment shader into machine code
	glCompileShader(fragmentShader);

	// create shader program object and get its reference
	GLuint shaderProgram = glCreateProgram();
	// attach shaders to program
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	// link all shaders together in shader program
	glLinkProgram(shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//create vertex array object and vertex buffer object
	GLuint VAO, VBO;

	// generate VAO
	glGenVertexArrays(1, &VAO);
	// generate VBO
	glGenBuffers(1, &VBO);

	// make VAO current vertex array object by binding it
	glBindVertexArray(VAO);
	// bind VBO specifiying GL_ARRAY_BUFFER
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// introduce vertices to VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// configure vertex attribute so the OpenGL knows how to read VBO
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	// enable vertex attribute so OpenGL sees it
	glEnableVertexAttribArray(0);

	// bind both VBO and VAO to 0 so we don't modify the VAO and VBO we created
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// specify window background color
	glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
	// clear back buffer and assign new color to it
	glClear(GL_COLOR_BUFFER_BIT);
	// swapping back buffer with front buffer
	glfwSwapBuffers(window);

	// program loop
	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		// tell OpenGL what program we want to use
		glUseProgram(shaderProgram);
		// bind VAO so OpenGL knows to use it
		glBindVertexArray(VAO);
		// draw the triangle
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glfwSwapBuffers(window);
		// taking care of all events like dragging window
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram);

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}