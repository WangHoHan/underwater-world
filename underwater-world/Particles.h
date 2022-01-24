#pragma once
#include "glm.hpp"
#include "freeglut.h"

void initParticles();
void handleAllParticleSources(glm::vec3 cameraPos, GLuint programParticles, glm::vec3 cameraSide, glm::vec3 cameraVertical, glm::mat4 cameraMatrix, glm::mat4 perspectiveMatrix);
void shutdownParticles();
void addParticleSource(glm::vec3 pos, double amount, float spread);