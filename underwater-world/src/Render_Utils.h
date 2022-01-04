#pragma once
#include "glm.hpp"
#include "ext.hpp"
#include "glew.h"
#include "objload.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Texture.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

namespace Core
{
	static inline glm::mat4 mat4_cast(const aiMatrix4x4& m) { return glm::transpose(glm::make_mat4(&m.a1)); }

	struct  Material {
		GLuint program;
		virtual void init_data() = 0;
	};

	struct DiffuseMaterial : Core::Material {
		GLuint texture;
		glm::vec3 lightDir;
		void init_data();

	};

	struct DiffuseSpecularMaterial : Core::Material {
		GLuint texture;
		GLuint textureSpecular;
		glm::vec3 lightDir;
		void init_data();

	};

	struct RenderContext
	{

		GLuint vertexArray;
		GLuint vertexBuffer;
		GLuint vertexIndexBuffer;
		Material* material;
		int size = 0;

		void initFromOBJ(obj::Model& model);

		void initFromAssimpMesh(aiMesh* mesh);

		void render();
	};

	struct RayContext : RenderContext {

		void render();
	};

	struct Node {
		std::vector<RenderContext> renderContexts;
		glm::mat4 matrix;
		int parent;
	};

	// vertexArray - jednowymiarowa tablica zawierajaca wartosci opisujace pozycje kolejnych wierzcholkow w jednym ciagu (x1, y1, z1, w1, x2, y2, z2, w2, ...)
	// numVertices - liczba wierzcholkow do narysowania
	// elementSize - liczba wartosci opisujacych pojedynczy wierzcholek (np. 3 gdy wierzcholek opisany jest trojka (x, y, z))
	void DrawVertexArray(const float * vertexArray, int numVertices, int elementSize);

	// indexArray - jednowymiarowa tablica zawierajaca indeksy wierzcholkow kolejnych trojkatow w jednym ciagu (t1_i1, t1_i2, t1_i3, t2_i1, t2_i2, t2_i3, ...)
	// numIndexes - liczba indeksow w tablicy indexArray
	void DrawVertexArrayIndexed(const float * vertexArray, const int * indexArray, int numIndexes, int elementSize);


	struct VertexAttribute
	{
		const void * Pointer;
		int Size;
	};

	struct VertexData
	{
		static const int MAX_ATTRIBS = 8;
		VertexAttribute Attribs[MAX_ATTRIBS];
		int NumActiveAttribs;
		int NumVertices;
	};

	// Ta funkcja sluzy do rysowania trojkatow, ktorych wierzcholki moga byc opisane wiecej niz jednym atrybutem.
	// Funkcja przyjmuje jako parametr strukture, w ktorej nalezy zawrzec wszystkie potrzebne dane.
	// 
	// Przykladowe wywolanie funkcji - narysowanie trojkata jak na pierwszych zajeciach:
	/*
	
	const float vertices[] = {
		0.25f,  0.25f, 0.0f, 1.0f,
		0.25f, -0.25f, 0.0f, 1.0f,
		-0.25f, -0.25f, 0.0f, 1.0f
	};
	
	Core::VertexData vertexData;
	vertexData.NumActiveAttribs = 1;			// Liczba uzywanych atrybutow wierzcholka
	vertexData.Attribs[0].Pointer = vertices;	// Wskaznik na dane zerowego atrybutu
	vertexData.Attribs[0].Size = 4;				// Wielkosc zerowego atrybutu (ilosc liczb opisujacych ten atrybut w pojedynczym wierzcholku)
	vertexData.NumVertices = 3;					// Liczba wierzcholkow do narysowania
	Core::DrawVertexArray(vertexData);

	*/
	void DrawVertexArray(const VertexData & data);

	void DrawContext(RenderContext& context);

	void initRay(RayContext& rayContext);

	void updateRayPos(RayContext& rayContext, std::vector<glm::vec3> keyPoints);
}