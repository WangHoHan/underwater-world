#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800
#define STB_IMAGE_IMPLEMENTATION
#define SKYBOX_PARAMETER 100.0f

#include "glew.h"
#include "freeglut.h"
#include "glm.hpp"
#include "gtx/matrix_decompose.hpp"
#include "ext.hpp"
#include <iostream>
#include <time.h>
#include <cctype>
#include <cmath>
#include <vector>
#include "Shader_Loader.h"
#include "Render_Utils.h"
#include "Texture.h"
#include "Camera.h"
#include "SOIL/stb_image_aug.h"

GLuint skyboxProgram, skyboxBuffer, terrainProgram, bubbleProgram, programColor, programTexture, programTexture2, textureStingray, textureTerrain, textureBubble, fishTexture, fish2Texture, fish3Texture, plantTexture, plant2Texture, plant3Texture, rockTexture;

unsigned int cubemapTexture, skyboxVAO;

float old_x, old_y = -1;
glm::vec3 cursorDiff;
glm::vec3 lightDir = glm::normalize(glm::vec3(1.0f, -10.f, -1.0f));
glm::vec3 cameraPos = glm::vec3(0, 10, 5);
glm::vec3 cameraDir; // Wektor "do przodu" kamery
glm::vec3 cameraSide; // Wektor "w bok" kamery
float cameraAngle = 0;

glm::quat rotation = glm::quat(1, 0, 0, 0);

glm::mat4 cameraMatrix, perspectiveMatrix;

Core::Shader_Loader shaderLoader;
Core::RenderContext stingrayContext;
Core::RenderContext terrainContext;
Core::RenderContext bubbleContext;
Core::RenderContext fishContext;
Core::RenderContext fish2Context;
Core::RenderContext fish3Context;
Core::RenderContext plantContext;
Core::RenderContext plant2Context;
Core::RenderContext plant3Context;
Core::RenderContext rockContext;

//std::string facesCubemap = "models/skybox/blue.jpg";
std::string facesCubemap[6] = {
	"models/skybox/right.jpg",
	"models/skybox/left.jpg",
	"models/skybox/top.jpg",
	"models/skybox/bottom.jpg",
	"models/skybox/front.jpg",
	"models/skybox/back.jpg"
};

std::vector<glm::vec3> bubblesPositions;
std::vector<glm::vec3> plantsPositions;

std::vector<glm::vec3> keyPointsFirstShoal({
	glm::vec3(1.0f, -5.0f, 1.0f),
	glm::vec3(0.0f, 0.0f, 1.0f),
	glm::vec3(10.0f, 0.0f, 5.0f),
	glm::vec3(45.0f, 5.0f, 15.0f),
	glm::vec3(70.0f, 7.0f, 15.0f),
	glm::vec3(70.0f, 7.5f, 25.0f),
	glm::vec3(65.0f, 7.0f, 35.0f),
	glm::vec3(95.0f, 7.0f, 35.0f),
	glm::vec3(125.0f, 7.0f, 35.0f),
	glm::vec3(155.0f, 7.0f, 30.0f),
	});

std::vector<glm::quat> keyRotationFirstShoal;

std::vector<glm::vec3> keyPointsSecondShoal({
	glm::vec3(60.0f, -2.0f, 1.0f),
	glm::vec3(55.0f, 0.0f, 1.0f),
	glm::vec3(40.0f, 0.0f, 5.0f),
	glm::vec3(35.0f, 1.0f, 5.0f),
	glm::vec3(30.0f, 1.0f, 5.0f),
	glm::vec3(28.0f, 1.2f, 7.0f),
	glm::vec3(21.0f, 1.5f, 15.0f),
	glm::vec3(19.0f, 1.5f, 25.0f),
	glm::vec3(15.0f, 2.0f, 35.0f),
	glm::vec3(14.0f, 2.0f, 45.0f),
	glm::vec3(9.0f, 3.0f, 60.0f),
	glm::vec3(3.0f, 3.0f, 80.0f),
	glm::vec3(3.0f, 4.0f, 100.0f)
	});

std::vector<glm::quat> keyRotationSecondShoal;

std::vector<glm::vec3> keyPointsThirdShoal({
	glm::vec3(1.0f, 0.0f, 60.0f),
	glm::vec3(10.0f, 0.0f, 65.0f),
	glm::vec3(45.0f, 0.0f, 75.0f),
	glm::vec3(55.0f, 0.0f, 150.0f),
	glm::vec3(60.0f, 0.0f, 200.0f),
	glm::vec3(65.0f, 0.0f, 200.0f),
	});

std::vector<glm::quat> keyRotationThirdShoal;


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
		cameraPos = nextPosition;
		break;
	case 's':
		nextPosition = cameraPos - (cameraDir * moveSpeed);
		cameraPos = nextPosition;
		break;
	case 'd':
		nextPosition = cameraPos + (cameraSide * moveSpeed);
		cameraPos = nextPosition;
		break;
	case 'a':
		nextPosition = cameraPos - (cameraSide * moveSpeed);
		cameraPos = nextPosition;
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


void drawObject(GLuint program, Core::RenderContext context, glm::mat4 modelMatrix)
{
	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;


	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
	glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);
	context.render();
}


void renderRecursive(std::vector<Core::Node>& nodes) {
	for (Core::Node node : nodes) {
		if (node.renderContexts.size() == 0) {
			continue;
		}

		glm::mat4 transformation = glm::mat4();
		transformation = node.matrix;
		Core::Node parentNode = node;
		while (parentNode.parent != -1) {
			parentNode = nodes[parentNode.parent];
			transformation = parentNode.matrix * transformation;
		}

		for (Core::RenderContext context : node.renderContexts) {
			auto program = context.material->program;
			glUseProgram(program);
			glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);
			context.material->init_data();
			drawObject(program, context, transformation);
		}
	}

}


glm::mat4 animationMatrix(float time, std::vector<glm::vec3> keyPoints, std::vector<glm::quat> keyRotation) {
	float speed = 1.;
	time = time * speed;
	std::vector<float> distances;
	float timeStep = 0;
	for (int i = 0; i < keyPoints.size() - 1; i++) {
		timeStep += (keyPoints[i] - keyPoints[i + 1]).length();
		distances.push_back((keyPoints[i] - keyPoints[i + 1]).length());
	}
	time = fmod(time, timeStep);


	int index = 0;

	while (distances[index] <= time) {
		time = time - distances[index];
		index += 1;
	}

	float t = time / distances[index];

	int size = keyPoints.size();

	glm::vec3 pos = glm::catmullRom(keyPoints[std::max(0, (index - 1) % size)], keyPoints[(index) % size], keyPoints[std::max(0, (index + 1) % size)], keyPoints[std::max(0, (index + 2) % size)], t);


	int keyRotationSize = keyRotation.size();

	auto a1 = keyRotation[index % keyRotationSize] * glm::exp(-(glm::log(glm::inverse(keyRotation[index % keyRotationSize]) * keyRotation[std::max(0, (index - 1) % keyRotationSize)]) + glm::log(glm::inverse(keyRotation[index % keyRotationSize]) * keyRotation[(index + 1) % keyRotationSize])) * glm::quat(1 / 4, 1 / 4, 1 / 4, 1 / 4));

	auto a2 = keyRotation[(index + 1) % keyRotationSize] * glm::exp(-(glm::log(glm::inverse(keyRotation[(index + 1) % keyRotationSize]) * keyRotation[index % keyRotationSize]) + glm::log(glm::inverse(keyRotation[(index + 1) % keyRotationSize]) * keyRotation[(index + 2) % keyRotationSize])) * glm::quat(1 / 4, 1 / 4, 1 / 4, 1 / 4));

	auto animationRotation = glm::squad(keyRotation[index % keyRotationSize], keyRotation[(index + 1) % keyRotationSize], a1, a2, t);

	glm::mat4 result = glm::translate(pos) * glm::mat4_cast(animationRotation);


	return result;

}


void renderScene()
{
	float time = glutGet(GLUT_ELAPSED_TIME) / 1000.f;
	cameraMatrix = createCameraMatrix();
	perspectiveMatrix = Core::createPerspectiveMatrix();
	glClearColor(0.0f, 0.109f, 0.447f, 1.0f);
	//glClearColor(0.219f, 0.407f, 0.705f, 1.0f);
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
	
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	for (int i = 0; i < bubblesPositions.size(); i++) {
		if (bubblesPositions[i].y > 15.0f) {
			bubblesPositions[i] = glm::vec3(rand() % 200 - 100, rand() % 10, rand() % 200 - 100);
		}
		bubblesPositions[i].y += 0.005f;
		drawObjectTexture(bubbleContext, glm::translate(bubblesPositions[i]) * glm::rotate(glm::radians(90.0f), glm::vec3(1, 0, 0)) * glm::scale(glm::vec3(0.02f)), textureBubble);
	}

	glDisable(GL_BLEND);


	glm::vec3  change1 = glm::vec3(0, 3, 0);
	glm::vec3  change2 = glm::vec3(0, 0, 0);
	glm::vec3  change3 = glm::vec3(3, 0, 0);
	glm::vec3  change4 = glm::vec3(0, 2, 1);
	for (int i = 0; i < 30; i++) {
		if (time > -10) {
			glm::mat4 matrix = animationMatrix(time + 15, keyPointsFirstShoal, keyRotationFirstShoal);
			drawObjectTexture(fishContext, matrix * glm::rotate(glm::radians(180.0f), glm::vec3(1, 0, 0)) * glm::rotate(glm::radians(180.0f), glm::vec3(0, 0, 1)) * glm::rotate(glm::radians(270.0f), glm::vec3(0, 0, 1)), fishTexture);
			drawObjectTexture(fishContext, matrix * glm::translate(change1) * glm::rotate(glm::radians(180.0f), glm::vec3(1, 0, 0)) * glm::rotate(glm::radians(180.0f), glm::vec3(0, 0, 1)) * glm::rotate(glm::radians(270.0f), glm::vec3(0, 0, 1)), fishTexture);
			drawObjectTexture(fishContext, matrix * glm::translate(change2) * glm::rotate(glm::radians(180.0f), glm::vec3(1, 0, 0)) * glm::rotate(glm::radians(180.0f), glm::vec3(0, 0, 1)) * glm::rotate(glm::radians(270.0f), glm::vec3(0, 0, 1)), fishTexture);
			drawObjectTexture(fishContext, matrix * glm::translate(change3) * glm::rotate(glm::radians(180.0f), glm::vec3(1, 0, 0)) * glm::rotate(glm::radians(180.0f), glm::vec3(0, 0, 1)) * glm::rotate(glm::radians(270.0f), glm::vec3(0, 0, 1)), fishTexture);
			drawObjectTexture(fishContext, matrix * glm::translate(change4) * glm::rotate(glm::radians(180.0f), glm::vec3(1, 0, 0)) * glm::rotate(glm::radians(180.0f), glm::vec3(0, 0, 1)) * glm::rotate(glm::radians(270.0f), glm::vec3(0, 0, 1)), fishTexture);


			matrix = animationMatrix(time + 15, keyPointsSecondShoal, keyRotationSecondShoal);
			drawObjectTexture(fish2Context, matrix * glm::translate(change1) * glm::rotate(glm::radians(0.0f), glm::vec3(1, 0, 0)) * glm::rotate(glm::radians(270.0f), glm::vec3(0,1,0)) * glm::rotate(glm::radians(0.0f), glm::vec3(0, 0, 1)), fish2Texture);
			drawObjectTexture(fish2Context, matrix * glm::translate(change2) * glm::rotate(glm::radians(0.0f), glm::vec3(1, 0, 0)) * glm::rotate(glm::radians(270.0f), glm::vec3(0,1,0)) * glm::rotate(glm::radians(0.0f), glm::vec3(0, 0, 1)), fish2Texture);
			drawObjectTexture(fish2Context, matrix * glm::translate(change3) * glm::rotate(glm::radians(0.0f), glm::vec3(1, 0, 0)) * glm::rotate(glm::radians(270.0f), glm::vec3(0,1,0)) * glm::rotate(glm::radians(0.0f), glm::vec3(0, 0, 1)), fish2Texture);
			drawObjectTexture(fish2Context, matrix * glm::translate(change4) * glm::rotate(glm::radians(0.0f), glm::vec3(1, 0, 0)) * glm::rotate(glm::radians(270.0f), glm::vec3(0,1,0)) * glm::rotate(glm::radians(0.0f), glm::vec3(0, 0, 1)), fish2Texture);

			matrix = animationMatrix(time + 15, keyPointsThirdShoal, keyRotationThirdShoal);
			drawObjectTexture(fish3Context, matrix * glm::translate(change1) * glm::rotate(glm::radians(1.0f), glm::vec3(1, 0, 0)) * glm::rotate(glm::radians(90.0f), glm::vec3(0, 1, 0)) * glm::rotate(glm::radians(180.0f), glm::vec3(0, 0, 1)), fish3Texture);
			drawObjectTexture(fish3Context, matrix * glm::translate(change2) * glm::rotate(glm::radians(1.0f), glm::vec3(1, 0, 0)) * glm::rotate(glm::radians(90.0f), glm::vec3(0, 1, 0)) * glm::rotate(glm::radians(180.0f), glm::vec3(0, 0, 1)), fish3Texture);
			drawObjectTexture(fish3Context, matrix * glm::translate(change3) * glm::rotate(glm::radians(1.0f), glm::vec3(1, 0, 0)) * glm::rotate(glm::radians(90.0f), glm::vec3(0, 1, 0)) * glm::rotate(glm::radians(180.0f), glm::vec3(0, 0, 1)), fish3Texture);
			drawObjectTexture(fish3Context, matrix * glm::translate(change4) * glm::rotate(glm::radians(1.0f), glm::vec3(1, 0, 0)) * glm::rotate(glm::radians(90.0f), glm::vec3(0, 1, 0)) * glm::rotate(glm::radians(180.0f), glm::vec3(0, 0, 1)), fish3Texture);
			time -= 3;
		}
	}

	for (int i = 0; i < 250; i++) {
		if (i % 3 == 0) {
			drawObjectTexture(plantContext, glm::translate(plantsPositions[i]) * glm::rotate(glm::radians(0.0f), glm::vec3(1, 0, 0)) * glm::scale(glm::vec3(0.2f)), plantTexture);
		} else if (i % 3 == 1) {
			drawObjectTexture(plant2Context, glm::translate(plantsPositions[i]) * glm::rotate(glm::radians(0.0f), glm::vec3(1, 0, 0)) * glm::scale(glm::vec3(1.0f)), plant2Texture);
		} else {
			drawObjectTexture(plant3Context, glm::translate(plantsPositions[i]) * glm::rotate(glm::radians(0.0f), glm::vec3(1, 0, 0)) * glm::scale(glm::vec3(1.0f)), plant3Texture);
		}
	}

	drawObjectTexture(rockContext, glm::translate(glm::vec3(1.0f, -5.0f, 1.0f)) * glm::rotate(glm::radians(0.0f), glm::vec3(1, 0, 0)) * glm::scale(glm::vec3(2.0f)), rockTexture);
	drawObjectTexture(rockContext, glm::translate(glm::vec3(60.0f, -5.0f, 1.0f)) * glm::rotate(glm::radians(0.0f), glm::vec3(1, 0, 0)) * glm::scale(glm::vec3(2.0f)), rockTexture);
	drawObjectTexture(rockContext, glm::translate(glm::vec3(1.0f, -5.0f, 60.0f)) * glm::rotate(glm::radians(0.0f), glm::vec3(1, 0, 0)) * glm::scale(glm::vec3(2.0f)), rockTexture);

	glutSwapBuffers();
}


void loadRecusive(const aiScene* scene, aiNode* node, std::vector<Core::Node>& nodes, std::vector<Core::Material*> materialsVector, int parentIndex) {
	int index = nodes.size();
	nodes.push_back(Core::Node());
	nodes[index].parent = parentIndex;
	nodes[index].matrix = Core::mat4_cast(node->mTransformation);
	for (int i = 0; i < node->mNumMeshes; i++) {
		Core::RenderContext context;
		context.initFromAssimpMesh(scene->mMeshes[node->mMeshes[i]]);
		context.material = materialsVector[scene->mMeshes[node->mMeshes[i]]->mMaterialIndex];
		nodes[index].renderContexts.push_back(context);
	}
	for (int i = 0; i < node->mNumChildren; i++) {
		loadRecusive(scene, node->mChildren[i], nodes, materialsVector, index);
	}
}


void loadRecusive(const aiScene* scene, std::vector<Core::Node>& nodes, std::vector<Core::Material*> materialsVector) {

	loadRecusive(scene, scene->mRootNode, nodes, materialsVector, -1);
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
		unsigned char* data = stbi_load(facesCubemap[i].c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);
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
			std::cout << "Cubemap tex failed to load at path: " << facesCubemap[i] << std::endl;
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


void initKeyRotation(std::vector<glm::vec3>& keyPoints, std::vector<glm::quat>& keyRotation) {
	glm::vec3 oldDirection = glm::vec3(0, 0, 1);
	glm::quat oldRotationCamera = glm::quat(1, 0, 0, 0);
	for (int i = 0; i < keyPoints.size() - 1; i++) {
		glm::vec3 newDirection = glm::normalize(keyPoints[i + 1] - keyPoints[i]);
		glm::quat rotation = glm::normalize(glm::rotationCamera(oldDirection, newDirection) * oldRotationCamera);
		keyRotation.push_back(rotation);
		oldDirection = newDirection;
		oldRotationCamera = rotation;
	}
	keyRotation.push_back(glm::quat(1, 0, 0, 0));
}

void init()
{
	glEnable(GL_DEPTH_TEST);
	programColor = shaderLoader.CreateProgram((char*) "shaders/shader_color.vert", (char*) "shaders/shader_color.frag");
	programTexture = shaderLoader.CreateProgram((char*) "shaders/shader_tex.vert", (char*) "shaders/shader_tex.frag");
	programTexture2 = shaderLoader.CreateProgram((char*) "shaders/shader_tex_2.vert", (char*) "shaders/shader_tex_2.frag");
	skyboxProgram = shaderLoader.CreateProgram((char *) "shaders/skybox.vert", (char *) "shaders/skybox.frag");

    loadCubemap();
	initSkybox();

	loadModelToContext("models/Ray.obj", stingrayContext);
	textureStingray = Core::LoadTexture("textures/Ray.png");

	loadModelToContext("models/CalidiousDesert_obj.obj", terrainContext);
	textureTerrain = Core::LoadTexture("textures/CalidiousDesert_diffuse.png");

	loadModelToContext("models/Oceans day.obj", bubbleContext);
;
	textureBubble = Core::LoadTexture("textures/texgen_0.png");
	for (int i = 0; i < 1500; i++) {
		bubblesPositions.push_back(glm::vec3(rand() % 200 - 100, rand() % 10, rand() % 200 - 100));
	}

	for (int i = 0; i < 1000; i++) {
		plantsPositions.push_back(glm::vec3(rand() % 200 - 100, -3.65f, rand() % 200 - 100));
	}

	loadModelToContext("models/GoldFish.obj", fishContext);
	fishTexture = Core::LoadTexture("textures/GoldFish.png");

	loadModelToContext("models/fish.obj", fish2Context);
	fish2Texture = Core::LoadTexture("textures/fish2.jpg");

	loadModelToContext("models/fish3.obj", fish3Context);
	fish3Texture = Core::LoadTexture("textures/fish3.jpeg");

	loadModelToContext("models/Aloe_mittel_normal.obj", plantContext);
	plantTexture = Core::LoadTexture("textures/Aloe_mittel.png");

	loadModelToContext("models/WustenAloe_mittel_Muster.obj", plant2Context);
	plant2Texture = Core::LoadTexture("textures/WustenAloe_mittel_Muster.png");

	loadModelToContext("models/LangeAloe.obj", plant3Context);
	plant3Texture = Core::LoadTexture("textures/LangeAloe.png");

	loadModelToContext("models/rock.obj", rockContext);
	rockTexture = Core::LoadTexture("textures/Rock_Albedo.tga.png");

	initKeyRotation(keyPointsFirstShoal, keyRotationFirstShoal);
	initKeyRotation(keyPointsSecondShoal, keyRotationSecondShoal);
	initKeyRotation(keyPointsThirdShoal, keyRotationThirdShoal);
}

void shutdown()
{
	shaderLoader.DeleteProgram(programColor);
	shaderLoader.DeleteProgram(programTexture);
	shaderLoader.DeleteProgram(programTexture2);
	shaderLoader.DeleteProgram(skyboxProgram);
	shaderLoader.DeleteProgram(terrainProgram);
	shaderLoader.DeleteProgram(bubbleProgram);
}

void idle()
{
	glutPostRedisplay();
}


int main(int argc, char** argv)
{
	srand(time(NULL));
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(200, 200);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Underwater Tibia");
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