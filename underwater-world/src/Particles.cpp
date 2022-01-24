// particle loading
#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

#include "glew.h"
#include "freeglut.h"
#include "glm.hpp"
#include "../glm/gtx/norm.hpp"

#include <algorithm>
#include <iostream>
#include <cmath>
#include <vector>


GLuint billboard_vertex_buffer;
GLuint particles_position_buffer;
GLuint particles_color_buffer;

GLuint particleTexture;

static GLfloat* g_particule_position_size_data;
static GLubyte* g_particule_color_data;

struct ParticleSource {
	glm::vec3 pos;
	double amount;
	float spread;
};

std::vector<ParticleSource> particleSources;

int ParticlesCount = 0;
double lastTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;

// CPU representation of a particle
struct Particle {
	glm::vec3 pos, speed;
	unsigned char r, g, b, a; // Color
	float size, angle, weight;
	float life; // Remaining life of the particle. if <0 : dead and unused.
	float cameradistance; // *Squared* distance to the camera. if dead : -1.0f

	bool operator<(const Particle& that) const {
		// Sort in reverse order : far particles drawn first.
		return this->cameradistance > that.cameradistance;
	}
};


void addParticleSource(glm::vec3 pos, double amount, float spread) {
	ParticleSource particleSource;
	particleSource.pos = pos;
	particleSource.amount = amount;
	particleSource.spread = spread;

	particleSources.push_back(particleSource);
}

const int MaxParticles = 100000;
Particle ParticlesContainer[MaxParticles];
int LastUsedParticle = 0;

// Finds a Particle in ParticlesContainer which isn't used yet.
// (i.e. life < 0);
int FindUnusedParticle() {

	for (int i = LastUsedParticle; i < MaxParticles; i++) {
		if (ParticlesContainer[i].life < 0) {
			LastUsedParticle = i;
			return i;
		}
	}

	for (int i = 0; i < LastUsedParticle; i++) {
		if (ParticlesContainer[i].life < 0) {
			LastUsedParticle = i;
			return i;
		}
	}

	return 0; // All particles are taken, override the first one
}

void SortParticles() {
	std::sort(&ParticlesContainer[0], &ParticlesContainer[MaxParticles]);
}

GLuint loadDDS(const char* imagepath)
{
	unsigned char header[124];

	FILE* fp;

	/* try to open the file */
	fp = fopen(imagepath, "rb");
	if (fp == NULL) {
		printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", imagepath); getchar();
		return 0;
	}

	/* verify the type of file */
	char filecode[4];
	fread(filecode, 1, 4, fp);
	if (strncmp(filecode, "DDS ", 4) != 0) {
		fclose(fp);
		return 0;
	}

	/* get the surface desc */
	fread(&header, 124, 1, fp);

	unsigned int height = *(unsigned int*)&(header[8]);
	unsigned int width = *(unsigned int*)&(header[12]);
	unsigned int linearSize = *(unsigned int*)&(header[16]);
	unsigned int mipMapCount = *(unsigned int*)&(header[24]);
	unsigned int fourCC = *(unsigned int*)&(header[80]);


	unsigned char* buffer;
	unsigned int bufsize;
	/* how big is it going to be including all mipmaps? */
	bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;
	buffer = (unsigned char*)malloc(bufsize * sizeof(unsigned char));
	fread(buffer, 1, bufsize, fp);
	/* close the file pointer */
	fclose(fp);

	unsigned int components = (fourCC == FOURCC_DXT1) ? 3 : 4;
	unsigned int format;
	switch (fourCC)
	{
	case FOURCC_DXT1:
		format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		break;
	case FOURCC_DXT3:
		format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		break;
	case FOURCC_DXT5:
		format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		break;
	default:
		free(buffer);
		return 0;
	}

	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
	unsigned int offset = 0;

	/* load the mipmaps */
	for (unsigned int level = 0; level < mipMapCount && (width || height); ++level)
	{
		unsigned int size = ((width + 3) / 4) * ((height + 3) / 4) * blockSize;
		glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height,
			0, size, buffer + offset);

		offset += size;
		width /= 2;
		height /= 2;

		// Deal with Non-Power-Of-Two textures. This code is not included in the webpage to reduce clutter.
		if (width < 1) width = 1;
		if (height < 1) height = 1;

	}

	free(buffer);

	return textureID;
}

void initParticles() {
	particleTexture = loadDDS("textures/particles/particle_original.DDS");

	g_particule_position_size_data = new GLfloat[MaxParticles * 4];
	g_particule_color_data = new GLubyte[MaxParticles * 4];

	for (int i = 0; i < MaxParticles; i++) {
		ParticlesContainer[i].life = -1.0f;
		ParticlesContainer[i].cameradistance = -1.0f;
	}

	// The VBO containing the 4 vertices of the particles.
	// Thanks to instancing, they will be shared by all particles.
	static const GLfloat g_vertex_buffer_data[] = {
	 -0.5f, -0.5f, 0.0f,
	 0.5f, -0.5f, 0.0f,
	 -0.5f, 0.5f, 0.0f,
	 0.5f, 0.5f, 0.0f,
	};

	glGenBuffers(1, &billboard_vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	// The VBO containing the positions and sizes of the particles
	glGenBuffers(1, &particles_position_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
	// Initialize with empty (NULL) buffer : it will be updated later, each frame.
	glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

	// The VBO containing the colors of the particles
	glGenBuffers(1, &particles_color_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
	// Initialize with empty (NULL) buffer : it will be updated later, each frame.
	glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);
}

void spawnParticles(double deltaTime, glm::vec3 sourcePosition, double amount, float spread) {

	// Generate 10 new particule each millisecond,
	// but limit this to 16 ms (60 fps), or if you have 1 long frame (1sec),
	// newparticles will be huge and the next frame even longer.
	int newparticles = (int)(deltaTime * amount);
	if (newparticles > (int)(0.016f * amount))
		newparticles = (int)(0.016f * amount);

	for (int i = 0; i < newparticles; i++) {
		int particleIndex = FindUnusedParticle();
		ParticlesContainer[particleIndex].life = 20.0f; // This particle will live 5 seconds.

		ParticlesContainer[particleIndex].pos = sourcePosition;

		glm::vec3 maindir = glm::vec3(0.0f, 0.4f, 0.0f);
		// Very bad way to generate a random direction; 
		// See for instance http://stackoverflow.com/questions/5408276/python-uniform-spherical-distribution instead,
		// combined with some user-controlled parameters (main direction, spread, etc)
		glm::vec3 randomdir = glm::vec3(
			(rand() % 2000 - 1000.0f) / 1000.0f,
			(rand() % 2000 - 1000.0f) / 1000.0f,
			(rand() % 2000 - 1000.0f) / 1000.0f
		);

		ParticlesContainer[particleIndex].speed = maindir + randomdir * spread;


		// Very bad way to generate a random color
		ParticlesContainer[particleIndex].r = 200;
		ParticlesContainer[particleIndex].g = 200;
		ParticlesContainer[particleIndex].b = 255;
		ParticlesContainer[particleIndex].a = (rand() % 256) / 3;

		ParticlesContainer[particleIndex].size = (rand() % 1000) / 100000.0f + 0.05f;

	}
}

void simulateParticles(glm::vec3 cameraPos, double deltaTime) {
	// Simulate all particles
	ParticlesCount = 0;
	for (int i = 0; i < MaxParticles; i++) {

		Particle& p = ParticlesContainer[i]; // shortcut

		if (p.life > 0.0f) {

			// Decrease life
			p.life -= deltaTime;
			if (p.life > 0.0f) {

				// Simulate simple physics : gravity only, no collisions
				// decreased acceleration - bubbles were too fast
				p.speed += glm::vec3(0.0f, 1.0f, 0.0f) * (float)deltaTime * 0.5f;
				p.pos += p.speed * (float)deltaTime;
				p.cameradistance = glm::length2(p.pos - cameraPos);
				//ParticlesContainer[i].pos += glm::vec3(0.0f,10.0f, 0.0f) * (float)delta;

				// Fill the GPU buffer
				g_particule_position_size_data[4 * ParticlesCount + 0] = p.pos.x;
				g_particule_position_size_data[4 * ParticlesCount + 1] = p.pos.y;
				g_particule_position_size_data[4 * ParticlesCount + 2] = p.pos.z;

				g_particule_position_size_data[4 * ParticlesCount + 3] = p.size;

				g_particule_color_data[4 * ParticlesCount + 0] = p.r;
				g_particule_color_data[4 * ParticlesCount + 1] = p.g;
				g_particule_color_data[4 * ParticlesCount + 2] = p.b;
				g_particule_color_data[4 * ParticlesCount + 3] = p.a;

			}
			else {
				// Particles that just died will be put at the end of the buffer in SortParticles();
				p.cameradistance = -1.0f;
			}

			ParticlesCount++;

		}
	}

	SortParticles();

	// printf("%d ", ParticlesCount);
}

void updateParticles() {
	// Update the buffers that OpenGL uses for rendering.
	// There are much more sophisticated means to stream data from the CPU to the GPU,
	// but this is outside the scope of this tutorial.
	// http://www.opengl.org/wiki/Buffer_Object_Streaming

	glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
	glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
	glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof(GLfloat) * 4, g_particule_position_size_data);

	glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
	glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
	glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof(GLubyte) * 4, g_particule_color_data);
}

void bindParticles(GLuint programParticles, glm::vec3 cameraSide, glm::vec3 cameraVertical, glm::mat4 cameraMatrix, glm::mat4 perspectiveMatrix) {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	glUseProgram(programParticles);

	// Vertex shader
	GLuint CameraRight_worldspace_ID = glGetUniformLocation(programParticles, "CameraRight_worldspace");
	GLuint CameraUp_worldspace_ID = glGetUniformLocation(programParticles, "CameraUp_worldspace");
	GLuint ViewProjMatrixID = glGetUniformLocation(programParticles, "VP");

	// fragment shader
	GLuint TextureID = glGetUniformLocation(programParticles, "myTextureSampler");

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, particleTexture);
	// Set our "myTextureSampler" sampler to user Texture Unit 0
	glUniform1i(TextureID, 0);

	// Same as the billboards tutorial
	glUniform3f(CameraRight_worldspace_ID, cameraSide.x, cameraSide.y, cameraSide.z);
	glUniform3f(CameraUp_worldspace_ID, cameraVertical.x, cameraVertical.y, cameraVertical.z);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix;
	glUniformMatrix4fv(ViewProjMatrixID, 1, GL_FALSE, (float*)&transformation);


	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
	glVertexAttribPointer(
		0, // attribute. No particular reason for 0, but must match the layout in the shader.
		3, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		0, // stride
		(void*)0 // array buffer offset
	);

	// 2nd attribute buffer : positions of particles' centers
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
	glVertexAttribPointer(
		1, // attribute. No particular reason for 1, but must match the layout in the shader.
		4, // size : x + y + z + size => 4
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		0, // stride
		(void*)0 // array buffer offset
	);

	// 3rd attribute buffer : particles' colors
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
	glVertexAttribPointer(
		2, // attribute. No particular reason for 1, but must match the layout in the shader.
		4, // size : r + g + b + a => 4
		GL_UNSIGNED_BYTE, // type
		GL_TRUE, // normalized? *** YES, this means that the unsigned char[4] will be accessible with a vec4 (floats) in the shader ***
		0, // stride
		(void*)0 // array buffer offset
	);
}

void renderParticles() {
	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, ParticlesCount);
	// These functions are specific to glDrawArrays*Instanced*.
	// The first parameter is the attribute buffer we're talking about.
	// The second parameter is the "rate at which generic vertex attributes advance when rendering multiple instances"
	// http://www.opengl.org/sdk/docs/man/xhtml/glVertexAttribDivisor.xml
	glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
	glVertexAttribDivisor(1, 1); // positions : one per quad (its center) -> 1
	glVertexAttribDivisor(2, 1); // color : one per quad -> 1

	// Draw the particules !
	// This draws many times a small triangle_strip (which looks like a quad).
	// This is equivalent to :
	// for(i in ParticlesCount) : glDrawArrays(GL_TRIANGLE_STRIP, 0, 4),
	// but faster.
	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, ParticlesCount);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}

void handleAllParticleSources(glm::vec3 cameraPos, GLuint programParticles, glm::vec3 cameraSide, glm::vec3 cameraVertical, glm::mat4 cameraMatrix, glm::mat4 perspectiveMatrix) {
	double currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	double delta = currentTime - lastTime;
	lastTime = currentTime;
	//std::cout << particleSources.size() << std::endl;
	for (ParticleSource particleSource : particleSources)
	{
		//std::cout << particleSource.pos.x << " " <<  particleSource.pos.y << "" << particleSource.pos.z << "" << std::endl;
		spawnParticles(delta, particleSource.pos, particleSource.amount, particleSource.spread);
	}

	simulateParticles(cameraPos, delta);
	updateParticles();
	bindParticles(programParticles, cameraSide, cameraVertical, cameraMatrix, perspectiveMatrix);
	renderParticles();
}

void shutdownParticles() {
	delete[] g_particule_position_size_data;
	glDeleteBuffers(1, &particles_color_buffer);
	glDeleteBuffers(1, &particles_position_buffer);
	glDeleteBuffers(1, &billboard_vertex_buffer);
}