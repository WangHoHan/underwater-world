#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800
#define STB_IMAGE_IMPLEMENTATION
#define SKYBOX_PARAMETER 50.0f

#include "glew.h"
#include "freeglut.h"
#include "glm.hpp"
#include "ext.hpp"
#include <iostream>
#include <cctype>
#include <cmath>
#include <vector>

#include "Shader_Loader.h"
#include "Render_Utils.h"
#include "Texture.h"
#include "Camera.h"
#include "SOIL/stb_image_aug.h"

GLuint skyboxProgram, skyboxBuffer, terrainProgram, programColor, programTexture, textureStingray, textureTerrain;

unsigned int cubemapTexture, skyboxVAO;

float old_x, old_y = -1;
glm::vec3 cursorDiff;
glm::vec3 lightDir = glm::normalize(glm::vec3(1.0f, -10.f, -1.0f));
glm::vec3 cameraPos = glm::vec3(0, 0, 5);
glm::vec3 cameraDir; // Wektor "do przodu" kamery
glm::vec3 cameraSide; // Wektor "w bok" kamery
float cameraAngle = 0;

glm::quat rotation = glm::quat(1, 0, 0, 0);

glm::mat4 cameraMatrix, perspectiveMatrix;

Core::Shader_Loader shaderLoader;
Core::RenderContext stingrayContext;
Core::RenderContext terrainContext;

std::string facesCubemap = "models/skybox/blue.jpg";

float skyboxBoundary = 49.5f;

float skyboxVertices[] = {
	-SKYBOX_PARAMETER,  SKYBOX_PARAMETER, -SKYBOX_PARAMETER,
	-SKYBOX_PARAMETER, -SKYBOX_PARAMETER, -SKYBOX_PARAMETER,
	 SKYBOX_PARAMETER, -SKYBOX_PARAMETER, -SKYBOX_PARAMETER,
	 SKYBOX_PARAMETER, -SKYBOX_PARAMETER, -SKYBOX_PARAMETER,
	 SKYBOX_PARAMETER,  SKYBOX_PARAMETER, -SKYBOX_PARAMETER,
	-SKYBOX_PARAMETER,  SKYBOX_PARAMETER, -SKYBOX_PARAMETER,

	-SKYBOX_PARAMETER, -SKYBOX_PARAMETER,  SKYBOX_PARAMETER,
	-SKYBOX_PARAMETER, -SKYBOX_PARAMETER, -SKYBOX_PARAMETER,
	-SKYBOX_PARAMETER,  SKYBOX_PARAMETER, -SKYBOX_PARAMETER,
	-SKYBOX_PARAMETER,  SKYBOX_PARAMETER, -SKYBOX_PARAMETER,
	-SKYBOX_PARAMETER,  SKYBOX_PARAMETER,  SKYBOX_PARAMETER,
	-SKYBOX_PARAMETER, -SKYBOX_PARAMETER,  SKYBOX_PARAMETER,

	 SKYBOX_PARAMETER, -SKYBOX_PARAMETER, -SKYBOX_PARAMETER,
	 SKYBOX_PARAMETER, -SKYBOX_PARAMETER,  SKYBOX_PARAMETER,
	 SKYBOX_PARAMETER,  SKYBOX_PARAMETER,  SKYBOX_PARAMETER,
	 SKYBOX_PARAMETER,  SKYBOX_PARAMETER,  SKYBOX_PARAMETER,
	 SKYBOX_PARAMETER,  SKYBOX_PARAMETER, -SKYBOX_PARAMETER,
	 SKYBOX_PARAMETER, -SKYBOX_PARAMETER, -SKYBOX_PARAMETER,

	-SKYBOX_PARAMETER, -SKYBOX_PARAMETER,  SKYBOX_PARAMETER,
	-SKYBOX_PARAMETER,  SKYBOX_PARAMETER,  SKYBOX_PARAMETER,
	 SKYBOX_PARAMETER,  SKYBOX_PARAMETER,  SKYBOX_PARAMETER,
	 SKYBOX_PARAMETER,  SKYBOX_PARAMETER,  SKYBOX_PARAMETER,
	 SKYBOX_PARAMETER, -SKYBOX_PARAMETER,  SKYBOX_PARAMETER,
	-SKYBOX_PARAMETER, -SKYBOX_PARAMETER,  SKYBOX_PARAMETER,

	-SKYBOX_PARAMETER,  SKYBOX_PARAMETER, -SKYBOX_PARAMETER,
	 SKYBOX_PARAMETER,  SKYBOX_PARAMETER, -SKYBOX_PARAMETER,
	 SKYBOX_PARAMETER,  SKYBOX_PARAMETER,  SKYBOX_PARAMETER,
	 SKYBOX_PARAMETER,  SKYBOX_PARAMETER,  SKYBOX_PARAMETER,
	-SKYBOX_PARAMETER,  SKYBOX_PARAMETER,  SKYBOX_PARAMETER,
	-SKYBOX_PARAMETER,  SKYBOX_PARAMETER, -SKYBOX_PARAMETER,

	-SKYBOX_PARAMETER, -SKYBOX_PARAMETER, -SKYBOX_PARAMETER,
	-SKYBOX_PARAMETER, -SKYBOX_PARAMETER,  SKYBOX_PARAMETER,
	 SKYBOX_PARAMETER, -SKYBOX_PARAMETER, -SKYBOX_PARAMETER,
	 SKYBOX_PARAMETER, -SKYBOX_PARAMETER, -SKYBOX_PARAMETER,
	-SKYBOX_PARAMETER, -SKYBOX_PARAMETER,  SKYBOX_PARAMETER,
	 SKYBOX_PARAMETER, -SKYBOX_PARAMETER,  SKYBOX_PARAMETER
};

bool isInSkybox(glm::vec3 nextPosition) {
	return nextPosition.z > -skyboxBoundary && nextPosition.z < skyboxBoundary && nextPosition.y > -skyboxBoundary &&
		nextPosition.y < skyboxBoundary && nextPosition.x < skyboxBoundary && nextPosition.x > -skyboxBoundary;
}


void keyboard(unsigned char key, int x, int y)
{
	key = tolower(key);
	int modifier = glutGetModifiers();

	float angleSpeed = 10.f;
	float moveSpeed = 0.1f;
	glm::vec3 nextPosition;

	if (modifier == GLUT_ACTIVE_SHIFT) {
		moveSpeed = 0.4f;
	}
	switch (key)
	{
	case 'z': cursorDiff.z -= angleSpeed; break;
	case 'x': cursorDiff.z += angleSpeed; break;
	case 'w':
		nextPosition = cameraPos + (cameraDir * moveSpeed);
		if (isInSkybox(nextPosition)) {
			cameraPos = nextPosition;
		}
		break;
	case 's':
		nextPosition = cameraPos - (cameraDir * moveSpeed);
		if (isInSkybox(nextPosition)) {
			cameraPos = nextPosition;
		}
		break;
	case 'd':
		nextPosition = cameraPos + (cameraSide * moveSpeed);
		if (isInSkybox(nextPosition)) {
			cameraPos = nextPosition;
		}
		break;
	case 'a':
		nextPosition = cameraPos - (cameraSide * moveSpeed);
		if (isInSkybox(nextPosition)) {
			cameraPos = nextPosition;
		}
		break;
	}
}

void mouse(int x, int y)
{
	if (old_x >= 0) {
		cursorDiff.x = x - old_x;
		cursorDiff.y = y - old_y;
	}
	old_x = x;
	old_y = y;
}

glm::mat4 createCameraMatrix()
{
	glm::quat rotation_x = glm::angleAxis(cursorDiff.y * 0.03f, glm::vec3(1, 0, 0));
	cursorDiff.y = 0;
	glm::quat rotation_y = glm::angleAxis(cursorDiff.x * 0.03f, glm::vec3(0, 1, 0));
	cursorDiff.x = 0;
	glm::quat rotation_z = glm::angleAxis(cursorDiff.z * 0.03f, glm::vec3(0, 0, 1));
	cursorDiff.z = 0;

	glm::quat rotationChange = rotation_x * rotation_y * rotation_z;
	rotation = glm::normalize(rotationChange * rotation);

	cameraDir = glm::inverse(rotation) * glm::vec3(0, 0, -1);
	cameraSide = glm::inverse(rotation) * glm::vec3(1, 0, 0);

	return Core::createViewMatrixQuat(cameraPos, rotation);
}

void drawObjectColor(Core::RenderContext context, glm::mat4 modelMatrix, glm::vec3 color)
{
	GLuint program = programColor;

	glUseProgram(program);

	glUniform3f(glGetUniformLocation(program, "objectColor"), color.x, color.y, color.z);
	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	Core::DrawContext(context);

	glUseProgram(0);
}

void drawObjectTexture(Core::RenderContext context, glm::mat4 modelMatrix, GLuint textureId)
{
	GLuint program = programTexture;

	glUseProgram(program);

	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);
	Core::SetActiveTexture(textureId, "textureSampler", program, 0);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	Core::DrawContext(context);

	glUseProgram(0);
}

void renderScene()
{
	cameraMatrix = createCameraMatrix();
	perspectiveMatrix = Core::createPerspectiveMatrix();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(skyboxProgram);
	glUniform1i(glGetUniformLocation(skyboxProgram, "skybox"), 0);
	glm::mat4 transformation = perspectiveMatrix * cameraMatrix;
	glUniformMatrix4fv(glGetUniformLocation(skyboxProgram, "projectionViewMatrix"), 1, GL_FALSE, (float*)&transformation);
	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	glm::mat4 stingrayInitialTransformation = glm::translate(glm::vec3(0, -0.5, -0.4)) * glm::rotate(glm::radians(180.0f), glm::vec3(0, 1, 0)) * glm::scale(glm::vec3(0.25f));
	glm::mat4 stingrayModelMatrix = glm::translate(cameraPos + cameraDir) * glm::mat4_cast(glm::inverse(rotation)) * stingrayInitialTransformation;
	
	drawObjectTexture(stingrayContext, stingrayModelMatrix, textureStingray);
	drawObjectTexture(terrainContext, glm::translate(glm::vec3(0, 130, 0)) * glm::rotate(glm::radians(90.0f), glm::vec3(1, 0, 0)) * glm::scale(glm::vec3(850.25f)), textureTerrain);
	glutSwapBuffers();
}

void loadModelToContext(std::string path, Core::RenderContext& context)
{
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}
	context.initFromAssimpMesh(scene->mMeshes[0]);
}

void loadCubemap()
{
	glGenTextures(1, &cubemapTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < 6; i++)
	{
		unsigned char* data = stbi_load(facesCubemap.c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << stbi_failure_reason() << std::endl;
			std::cout << "Cubemap tex failed to load at path: " << facesCubemap << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

}

void initSkybox()
{
	glGenVertexArrays(1, &skyboxVAO);
	glBindVertexArray(skyboxVAO);

	glGenBuffers(1, &skyboxBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

	GLuint vPosition = glGetAttribLocation(skyboxProgram, "aPos");
	glEnableVertexAttribArray(vPosition);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(skyboxVertices), skyboxVertices);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}

void init()
{
	glEnable(GL_DEPTH_TEST);
	programColor = shaderLoader.CreateProgram((char*) "shaders/shader_color.vert", (char*) "shaders/shader_color.frag");
	programTexture = shaderLoader.CreateProgram((char*) "shaders/shader_tex.vert", (char*) "shaders/shader_tex.frag");
	skyboxProgram = shaderLoader.CreateProgram((char *) "shaders/skybox.vert", (char *) "shaders/skybox.frag");
    loadCubemap();
	loadModelToContext("models/Ray.obj", stingrayContext);
	textureStingray = Core::LoadTexture("textures/Ray.png");
	loadModelToContext("models/CalidiousDesert_obj.obj", terrainContext);
	textureTerrain = Core::LoadTexture("textures/CalidiousDesert_diffuse.png");
	initSkybox();
}

void shutdown()
{
	shaderLoader.DeleteProgram(programColor);
	shaderLoader.DeleteProgram(programTexture);
	shaderLoader.DeleteProgram(skyboxProgram);
	shaderLoader.DeleteProgram(terrainProgram);
}

void idle()
{
	glutPostRedisplay();
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(200, 200);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Underwater World");
	glewInit();
	glutSetCursor(GLUT_CURSOR_NONE);
	init();
	glutKeyboardFunc(keyboard);
	glutPassiveMotionFunc(mouse);
	glutDisplayFunc(renderScene);
	glutIdleFunc(idle);

	glutMainLoop();

	shutdown();

	return 0;
}